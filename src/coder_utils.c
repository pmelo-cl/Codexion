/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:34:38 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 15:20:10 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_coders(t_coder **coders, t_dongle *dongles,
		t_config *cfg, t_shared *shared)
{
	int	i;

	*coders = malloc(sizeof(t_coder) * cfg->num_coders);
	if (!*coders)
		return (0);
	i = 0;
	while (i < cfg->num_coders)
	{
		(*coders)[i].id = i + 1;
		(*coders)[i].state = STATE_IDLE;
		(*coders)[i].compiles_done = 0;
		(*coders)[i].last_compile_start = 0;
		(*coders)[i].deadline = cfg->time_to_burnout;
		(*coders)[i].dongle_left = &dongles[i];
		(*coders)[i].dongle_right = &dongles[(i + 1) % cfg->num_coders];
		(*coders)[i].cfg = cfg;
		(*coders)[i].shared = shared;
		(*coders)[i].all_dongles = dongles;
		pthread_mutex_init(&(*coders)[i].state_mutex, NULL);
		i++;
	}
	return (1);
}

void	destroy_coders(t_coder *coders, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_mutex_destroy(&coders[i].state_mutex);
		i++;
	}
	free(coders);
}

int	compile_done_reached(t_coder *coder)
{
	int	done;

	pthread_mutex_lock(&coder->state_mutex);
	done = coder->compiles_done >= coder->cfg->compiles_required;
	pthread_mutex_unlock(&coder->state_mutex);
	return (done);
}
