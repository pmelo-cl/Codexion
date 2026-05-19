/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue_edf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 17:11:47 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:13:35 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	insert_edf_node(t_dongle *d,
				t_wait_node *node,
				t_wait_node *prev,
				t_wait_node *curr)
{
	if (!prev)
	{
		node->next = d->wait_head;
		d->wait_head = node;
	}
	else
	{
		prev->next = node;
		node->next = curr;
	}
	if (!curr)
		d->wait_tail = node;
}

void	enqueue_edf(t_dongle *d, t_wait_node *node)
{
	t_wait_node	*curr;
	t_wait_node	*prev;

	if (!d->wait_head)
	{
		d->wait_head = node;
		d->wait_tail = node;
		return ;
	}
	curr = d->wait_head;
	prev = NULL;
	while (curr && (curr->deadline < node->deadline
			|| (curr->deadline == node->deadline
				&& curr->arrival_time < node->arrival_time)))
	{
		prev = curr;
		curr = curr->next;
	}
	insert_edf_node(d, node, prev, curr);
}
