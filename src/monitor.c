/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:21:12 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 18:06:33 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	destroy_monitor(t_monitor *mon)
{
	if (!mon)
		return ;
	pthread_mutex_destroy(&mon->mutex);
	pthread_cond_destroy(&mon->cond);
	free(mon);
}

int	init_monitor(t_monitor *mon, t_sim *sim)
{
	if (!mon || !sim)
		return (1);

	*mon = (t_monitor){0};

	mon->coders = sim->coders;
	mon->dongles = sim->dongles;
	mon->cfg = &sim->cfg;
	mon->shared = &sim->shared;

	if (pthread_mutex_init(&mon->mutex, NULL) != 0)
		return (1);

	if (pthread_cond_init(&mon->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&mon->mutex);
		return (1);
	}

	return (0);
}

void	*monitor_routine(void *arg)
{
	t_monitor	*mon;

	mon = (t_monitor *)arg;
	pthread_mutex_lock(&mon->mutex);
	while (1)
	{
		if (!check_running(mon))
			break ;
		if (!handle_completion(mon))
			break ;
		wait_next_deadline(mon);
		if (!handle_burnout(mon))
			break ;
		if (!handle_completion(mon))
			break ;
	}
	pthread_mutex_unlock(&mon->mutex);
	return (NULL);
}
