/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:33:37 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 16:36:58 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_sim	sim;
	int		created;

	if (!parse_args(argc, argv, &sim.cfg))
		return (1);
	sim.cfg.start_time = get_abs_time_ms();
	if (!init_simulation(&sim))
		return (1);
	created = start_simulation(&sim);
	cleanup_simulation(&sim, created);
	return (0);
}
