/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:36:21 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:39:08 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_running(t_monitor *mon)
{
	pthread_mutex_lock(&mon->shared->running_mutex);
	if (!mon->shared->running)
	{
		pthread_mutex_unlock(&mon->shared->running_mutex);
		return (0);
	}
	pthread_mutex_unlock(&mon->shared->running_mutex);
	return (1);
}

int	handle_completion(t_monitor *mon)
{
	if (all_completed(mon))
	{
		stop_simulation(mon);
		return (0);
	}
	return (1);
}

void	wait_next_deadline(t_monitor *mon)
{
	long long		nd;
	long long		wait_ms;
	struct timespec	ts;

	nd = next_deadline(mon);
	if (nd == -1)
	{
		pthread_cond_wait(&mon->cond, &mon->mutex);
		return ;
	}
	wait_ms = nd - (get_abs_time_ms() - mon->cfg->start_time);
	if (wait_ms > 0)
	{
		get_abs_timespec(&ts, wait_ms);
		pthread_cond_timedwait(&mon->cond, &mon->mutex, &ts);
	}
}

int	handle_burnout(t_monitor *mon)
{
	if (check_burnout(mon))
	{
		stop_simulation(mon);
		return (0);
	}
	return (1);
}
