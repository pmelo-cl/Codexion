/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_stop.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:55:00 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:51:28 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	stop_simulation(t_monitor *mon)
{
	int	i;

	pthread_mutex_lock(&mon->shared->running_mutex);
	mon->shared->running = 0;
	pthread_mutex_unlock(&mon->shared->running_mutex);
	i = 0;
	while (i < mon->cfg->num_coders)
	{
		pthread_mutex_lock(&mon->dongles[i].mutex);
		pthread_cond_broadcast(&mon->dongles[i].cond);
		pthread_mutex_unlock(&mon->dongles[i].mutex);
		i++;
	}
}
