/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_threads.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 16:34:40 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 16:43:09 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	start_monitor(t_monitor *mon, t_shared *shared)
{
	if (pthread_create(&mon->thread, NULL,
			monitor_routine, mon) != 0)
	{
		fprintf(stderr,
			"Error: failed to create monitor thread\n");
		shared->running = 0;
		return (0);
	}
	return (1);
}

static int	handle_coder_error(t_shared *shared, int i)
{
	fprintf(stderr,
		"Error: failed to create coder %d thread\n",
		i + 1);
	pthread_mutex_lock(&shared->running_mutex);
	shared->running = 0;
	pthread_mutex_unlock(&shared->running_mutex);
	return (i);
}

static int	start_coders(t_coder *coders,
			t_config *cfg,
			t_shared *shared)
{
	int	i;

	i = 0;
	while (i < cfg->num_coders)
	{
		if (pthread_create(&coders[i].thread, NULL,
				coder_routine, &coders[i]) != 0)
			return (handle_coder_error(shared, i));
		i++;
	}
	return (i);
}

int	start_simulation(t_sim *sim)
{
	int	created;

	start_monitor(&sim->mon, &sim->shared);
	created = start_coders(sim->coders,
			&sim->cfg, &sim->shared);
	return (created);
}
