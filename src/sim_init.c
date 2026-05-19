/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_init.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 16:35:28 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:39:23 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	init_shared(t_shared *shared)
{
	pthread_mutex_init(&shared->print_mutex, NULL);
	pthread_mutex_init(&shared->running_mutex, NULL);
	shared->running = 1;
}

int	init_simulation(t_sim *sim)
{
	init_shared(&sim->shared);
	if (!init_dongles(&sim->dongles, &sim->cfg))
		return (fprintf(stderr,
				"Error: init_dongles failed\n"), 0);
	if (!init_coders(&sim->coders,
			sim->dongles, &sim->cfg, &sim->shared))
	{
		destroy_dongles(sim->dongles,
			sim->cfg.num_coders);
		fprintf(stderr, "Error: init_coders failed\n");
		return (0);
	}
	if (!init_monitor(&sim->mon, sim))
	{
		destroy_coders(sim->coders, sim->cfg.num_coders);
		destroy_dongles(sim->dongles, sim->cfg.num_coders);
		return (0);
	}
	return (1);
}
