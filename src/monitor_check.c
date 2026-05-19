/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor_check.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:45:00 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:39:03 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	all_completed(t_monitor *mon)
{
	int	i;
	int	done;

	i = 0;
	while (i < mon->cfg->num_coders)
	{
		pthread_mutex_lock(&mon->coders[i].state_mutex);
		done = mon->coders[i].compiles_done;
		pthread_mutex_unlock(&mon->coders[i].state_mutex);
		if (done < mon->cfg->compiles_required)
			return (0);
		i++;
	}
	return (1);
}

int	check_burnout(t_monitor *mon)
{
	int			i;
	long long	now;

	now = get_abs_time_ms() - mon->cfg->start_time;
	i = 0;
	while (i < mon->cfg->num_coders)
	{
		pthread_mutex_lock(&mon->coders[i].state_mutex);
		if (mon->coders[i].state == STATE_IDLE
			&& mon->coders[i].compiles_done < mon->cfg->compiles_required
			&& mon->coders[i].deadline <= now)
		{
			print_log(mon->shared, mon->cfg->start_time,
				mon->coders[i].id, "burned out");
			pthread_mutex_unlock(&mon->coders[i].state_mutex);
			return (1);
		}
		pthread_mutex_unlock(&mon->coders[i].state_mutex);
		i++;
	}
	return (0);
}

long long	next_deadline(t_monitor *mon)
{
	long long	next;
	int			i;

	next = -1;
	i = 0;
	while (i < mon->cfg->num_coders)
	{
		pthread_mutex_lock(&mon->coders[i].state_mutex);
		if (mon->coders[i].state == STATE_IDLE
			&& mon->coders[i].compiles_done < mon->cfg->compiles_required)
		{
			if (next == -1 || mon->coders[i].deadline < next)
				next = mon->coders[i].deadline;
		}
		pthread_mutex_unlock(&mon->coders[i].state_mutex);
		i++;
	}
	return (next);
}