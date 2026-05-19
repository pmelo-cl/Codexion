/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:33:09 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 15:20:44 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	coder_set_state(t_coder *coder, int state)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->state = state;
	if (state == STATE_IDLE)
		coder->deadline = coder->last_compile_start
			+ coder->cfg->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
}

static void	take_ordered_dongles(t_coder *coder,
				t_dongle **first, t_dongle **second)
{
	if (coder->dongle_left < coder->dongle_right)
	{
		*first = coder->dongle_left;
		*second = coder->dongle_right;
	}
	else
	{
		*first = coder->dongle_right;
		*second = coder->dongle_left;
	}
}

static int	is_running(t_coder *coder)
{
	int	running;

	pthread_mutex_lock(&coder->shared->running_mutex);
	running = coder->shared->running;
	pthread_mutex_unlock(&coder->shared->running_mutex);
	return (running);
}

static int	try_compile(t_coder *coder)
{
	t_dongle	*d1;
	t_dongle	*d2;
	long long	now;

	coder_set_state(coder, STATE_IDLE);
	take_ordered_dongles(coder, &d1, &d2);
	if (!dongle_take(d1, coder))
		return (0);
	if (!dongle_take(d2, coder))
	{
		dongle_release(d1, coder->cfg, coder->shared, coder->id);
		return (0);
	}
	now = get_abs_time_ms() - coder->cfg->start_time;
	coder_set_state(coder, STATE_COMPILING);
	coder->last_compile_start = now;
	coder->compiles_done++;
	print_log(coder->shared, coder->cfg->start_time,
		coder->id, "is compiling");
	ms_sleep(coder->cfg->time_to_compile);
	dongle_release(d1, coder->cfg, coder->shared, coder->id);
	dongle_release(d2, coder->cfg, coder->shared, coder->id);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	while (is_running(coder))
	{
		if (!try_compile(coder))
			break ;
		if (coder->compiles_done >= coder->cfg->compiles_required)
			break ;
		if (!is_running(coder))
			break ;
		coder_set_state(coder, STATE_DEBUGGING);
		print_log(coder->shared, coder->cfg->start_time,
			coder->id, "is debugging");
		ms_sleep(coder->cfg->time_to_debug);
		if (!is_running(coder))
			break ;
		coder_set_state(coder, STATE_REFACTORING);
		print_log(coder->shared, coder->cfg->start_time,
			coder->id, "is refactoring");
		ms_sleep(coder->cfg->time_to_refactor);
	}
	return (NULL);
}
