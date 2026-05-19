/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_cleanup.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 16:35:09 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:39:33 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	destroy_shared(t_shared *shared)
{
	pthread_mutex_destroy(&shared->print_mutex);
	pthread_mutex_destroy(&shared->running_mutex);
}

static void	join_coders(t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(coders[i].thread, NULL);
		i++;
	}
}

void	cleanup_simulation(t_sim *sim, int created)
{
	join_coders(sim->coders, created);
	pthread_join(sim->mon.thread, NULL);
	destroy_monitor(&sim->mon);
	destroy_coders(sim->coders, sim->cfg.num_coders);
	destroy_dongles(sim->dongles, sim->cfg.num_coders);
	destroy_shared(&sim->shared);
}
