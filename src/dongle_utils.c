/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 15:56:13 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 16:00:57 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_dongle **dongles, t_config *cfg)
{
	int	i;

	*dongles = malloc(sizeof(t_dongle) * cfg->num_coders);
	if (!*dongles)
		return (0);
	i = 0;
	while (i < cfg->num_coders)
	{
		pthread_mutex_init(&(*dongles)[i].mutex, NULL);
		pthread_cond_init(&(*dongles)[i].cond, NULL);
		(*dongles)[i].occupied = 0;
		(*dongles)[i].cooldown_end = 0;
		(*dongles)[i].wait_head = NULL;
		(*dongles)[i].wait_tail = NULL;
		i++;
	}
	return (1);
}

void	dongle_release(t_dongle *d, t_config *cfg, t_shared *shared,
		int coder_id)
{
	(void)shared;
	(void)coder_id;
	pthread_mutex_lock(&d->mutex);
	d->occupied = 0;
	d->cooldown_end = get_abs_time_ms() - cfg->start_time
		+ cfg->dongle_cooldown;
	if (d->wait_head)
		pthread_cond_broadcast(&d->cond);
	pthread_mutex_unlock(&d->mutex);
}

void	destroy_dongles(t_dongle *dongles, int count)
{
	int			i;
	t_wait_node	*node;
	t_wait_node	*tmp;

	i = 0;
	while (i < count)
	{
		node = dongles[i].wait_head;
		while (node)
		{
			tmp = node;
			node = node->next;
			free(tmp);
		}
		pthread_mutex_destroy(&dongles[i].mutex);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
	free(dongles);
}
