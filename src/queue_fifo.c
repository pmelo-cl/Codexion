/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_fifo.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:33:51 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:13:39 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

#include "codexion.h"

static void	enqueue_fifo(t_dongle *d, t_wait_node *node)
{
	if (!d->wait_head)
	{
		d->wait_head = node;
		d->wait_tail = node;
		return ;
	}
	d->wait_tail->next = node;
	d->wait_tail = node;
}

void	enqueue_wait(t_dongle *d,
			t_wait_node *node,
			int scheduler)
{
	if (scheduler == SCHED_FIFO_TYPE)
		enqueue_fifo(d, node);
	else
		enqueue_edf(d, node);
}

t_wait_node	*dequeue_wait(t_dongle *d)
{
	t_wait_node	*first;

	if (!d->wait_head)
		return (NULL);
	first = d->wait_head;
	d->wait_head = first->next;
	if (!d->wait_head)
		d->wait_tail = NULL;
	first->next = NULL;
	return (first);
}

void	remove_wait_node(t_dongle *d, t_wait_node *node)
{
	t_wait_node	*curr;
	t_wait_node	*prev;

	curr = d->wait_head;
	prev = NULL;
	while (curr && curr != node)
	{
		prev = curr;
		curr = curr->next;
	}
	if (!curr)
		return ;
	if (!prev)
		d->wait_head = curr->next;
	else
		prev->next = curr->next;
	if (d->wait_tail == curr)
		d->wait_tail = prev;
	curr->next = NULL;
}
