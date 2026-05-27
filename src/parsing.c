/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 16:03:27 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 16:28:39 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_int(const char *str, int *value)
{
	int	i;
	int	val;

	i = 0;
	if (!str || !str[0])
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	if (i > 10)
		return (0);
	val = atoi(str);
	if (val < 0)
		return (0);
	*value = val;
	return (1);
}

static int	parse_positive(char *arg, int *dst, char *msg, int limit)
{
	int	tmp;

	if (!is_valid_int(arg, &tmp) || tmp < limit)
	{
		fprintf(stderr, "%s\n", msg);
		return (0);
	}
	*dst = tmp;
	return (1);
}

static int	parse_args_values(char **argv, t_config *cfg)
{
	if (!parse_positive(argv[1], &cfg->num_coders,
			"Error: num_coders must be >= 1\n", 1))
		return (0);
	if (!parse_positive(argv[2], &cfg->time_to_burnout,
			"Error: time_to_burnout must be > 0", 1))
		return (0);
	if (!parse_positive(argv[3], &cfg->time_to_compile,
			"Error: time_to_compile must be > 0", 1))
		return (0);
	if (!parse_positive(argv[4], &cfg->time_to_debug,
			"Error: time_to_debug must be > 0", 1))
		return (0);
	if (!parse_positive(argv[5], &cfg->time_to_refactor,
			"Error: time_to_refactor must be > 0", 1))
		return (0);
	if (!parse_positive(argv[6], &cfg->compiles_required,
			"Error: compiles_required must be > 0", 1))
		return (0);
	if (!parse_positive(argv[7], &cfg->dongle_cooldown,
			"Error: dongle_cooldown must be >= 0", 0))
		return (0);
	return (1);
}

static int	parse_scheduler(char *arg, t_config *cfg)
{
	if (strcmp(arg, "fifo") == 0)
		cfg->scheduler = SCHED_FIFO_TYPE;
	else if (strcmp(arg, "edf") == 0)
		cfg->scheduler = SCHED_EDF_TYPE;
	else
	{
		fprintf(stderr,
			"Error: scheduler must be fifo or edf\n");
		return (0);
	}
	return (1);
}

int	parse_args(int argc, char **argv, t_config *cfg)
{
	if (argc != 9)
	{
		fprintf(stderr,
			"Usage: %s num_coders time_to_burnout "
			"time_to_compile time_to_debug "
			"time_to_refactor compiles_required "
			"dongle_cooldown scheduler\n",
			argv[0]);
		return (0);
	}
	if (!parse_args_values(argv, cfg))
		return (0);
	if (!parse_scheduler(argv[8], cfg))
		return (0);
	return (1);
}