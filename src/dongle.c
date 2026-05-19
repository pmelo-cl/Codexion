/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:33:30 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 16:01:06 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	assign_dongle(t_dongle *d, t_wait_node *node)
{
	d->occupied = 1;
	remove_wait_node(d, node);
	return (1);
}

static int	unlock_and_cleanup(t_dongle *d,
				t_wait_node *node,
				t_coder *coder)
{
	pthread_mutex_unlock(&coder->shared->running_mutex);
	remove_wait_node(d, node);
	free(node);
	pthread_mutex_unlock(&d->mutex);
	return (0);
}

static int	init_wait_node(t_dongle *d, t_coder *coder,
				t_wait_node **node)
{
	*node = malloc(sizeof(t_wait_node));
	if (!*node)
		return (0);
	(*node)->coder_id = coder->id;
	(*node)->deadline = coder->deadline;
	(*node)->arrival_time = get_abs_time_ms()
		- coder->cfg->start_time;
	(*node)->next = NULL;
	pthread_mutex_lock(&d->mutex);
	enqueue_wait(d, *node, coder->cfg->scheduler);
	return (1);
}

static int	wait_for_dongle(t_dongle *d, t_coder *coder,
				t_wait_node *node)
{
	struct timespec	abs_time;
	long long		now;
	long long		wait_ms;

	while (1)
	{
		pthread_mutex_lock(&coder->shared->running_mutex);
		if (!coder->shared->running)
			return (unlock_and_cleanup(d, node, coder));
		pthread_mutex_unlock(&coder->shared->running_mutex);
		if (d->wait_head == node && !d->occupied)
		{
			now = get_abs_time_ms() - coder->cfg->start_time;
			if (now >= d->cooldown_end)
				return (assign_dongle(d, node));
			wait_ms = d->cooldown_end - now;
			get_abs_timespec(&abs_time, wait_ms);
			pthread_cond_timedwait(&d->cond, &d->mutex, &abs_time);
		}
		else
			pthread_cond_wait(&d->cond, &d->mutex);
	}
}

int	dongle_take(t_dongle *d, t_coder *coder)
{
	t_wait_node	*node;

	if (!init_wait_node(d, coder, &node))
		return (0);
	if (!wait_for_dongle(d, coder, node))
		return (0);
	free(node);
	pthread_mutex_unlock(&d->mutex);
	print_log(coder->shared, coder->cfg->start_time,
		coder->id, "has taken a dongle");
	return (1);
}
