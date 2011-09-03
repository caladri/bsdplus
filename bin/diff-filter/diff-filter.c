#include <err.h>
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum rule_type {
	AllowRule,
	DenyRule,
};

struct rule {
	enum rule_type r_type;
	regex_t r_regex;
	struct rule *r_next;
};

static int diff_filter(FILE *, enum rule_type, struct rule *);
static int diff_filter_file(const char *, enum rule_type, struct rule *);
static struct rule *rule_add(struct rule *, enum rule_type, const char *);
static enum rule_type rules_run(const char *, enum rule_type, struct rule *);
static void usage(void);

int
main(int argc, char *argv[])
{
	enum rule_type default_rule;
	struct rule *rules;
	const char *file;
	int error;
	int ch;

	default_rule = AllowRule;
	rules = NULL;

	while ((ch = getopt(argc, argv, "ADa:d:")) != -1) {
		switch (ch) {
		case 'A':
			default_rule = AllowRule;
			break;
		case 'D':
			default_rule = DenyRule;
			break;
		case 'a':
			rules = rule_add(rules, AllowRule, optarg);
			break;
		case 'd':
			rules = rule_add(rules, DenyRule, optarg);
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	if (argc == 0) {
		error = diff_filter(stdin, default_rule, rules);
		if (error != 0)
			errc(1, error, "diff_filter");
	} else {
		int errors;

		errors = 0;

		while (argc--) {
			file = *argv++;
			error = diff_filter_file(file, default_rule, rules);
			if (error != 0) {
				warnc(error, "diff_filter(%s)", file);
				errors++;
			}
		}
		if (errors > 0)
			errx(1, "%u files had errors", errors);
	}

	return (0);
}

static int
diff_filter(FILE *file, enum rule_type default_rule, struct rule *rules)
{
	enum rule_type state;
	char *line;
	size_t len;

	state = default_rule;

	while ((line = fgetln(file, &len)) != NULL) {
		if (len == 0)
			continue;
		line[len - 1] = '\0';

		if (strncmp(line, "Index: ", 7) == 0)
			state = rules_run(line + 7, default_rule, rules);

		if (state == AllowRule)
			fprintf(stdout, "%s\n", line);
	}
	if (ferror(file)) {
		if (errno == 0)
			return (EIO);
		return (errno);
	}

	return (0);
}

static int
diff_filter_file(const char *name, enum rule_type default_rule, struct rule *rules)
{
	FILE *file;
	int error;

	file = fopen(name, "r");
	if (file == NULL)
		return (errno);

	error = diff_filter(file, default_rule, rules);

	fclose(file);

	return (error);
}

static struct rule *
rule_add(struct rule *rules, enum rule_type type, const char *string)
{
	struct rule *rule;
	int rv;

	rule = malloc(sizeof *rule);
	rule->r_type = type;
	rule->r_next = NULL;

	rv = regcomp(&rule->r_regex, string, REG_NOSUB | REG_EXTENDED);
	if (rv != 0)
		err(1, "couldn't compile regex \"%s\"", string);

	if (rules == NULL)
		return (rule);

	while (rules->r_next != NULL)
		rules = rules->r_next;
	rules->r_next = rule;
	return (rules);
}

static enum rule_type
rules_run(const char *line, enum rule_type default_rule, struct rule *rules)
{
	struct rule *rule;
	int rv;

	for (rule = rules; rule != NULL; rule = rule->r_next) {
		rv = regexec(&rule->r_regex, line, 0, NULL, 0);
		if (rv == 0)
			return (rule->r_type);
		if (rv == REG_NOMATCH)
			continue;
		errx(1, "regexec failed: %d", rv);
	}

	return (default_rule);
}

static void
usage(void)
{
	fprintf(stderr,
"diff-filter [-A | -D] [-a allow_regex] [-d deny_regex] file ...\n");
	exit(1);
}
