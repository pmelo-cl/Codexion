/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:33:55 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 14:34:11 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_abs_time_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) != 0)
	{
		perror("gettimeofday");
		exit(EXIT_FAILURE);
	}
	return (tv.tv_sec * 1000LL + tv.tv_usec / 1000);
}

void	ms_sleep(int ms)
{
	struct timespec	req;
	struct timespec	rem;

	req.tv_sec = ms / 1000;
	req.tv_nsec = (ms % 1000) * 1000000;
	while (nanosleep(&req, &rem) == -1 && errno == EINTR)
		req = rem;
}

void	get_abs_timespec(struct timespec *ts, long long timeout_ms)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec + timeout_ms / 1000;
	ts->tv_nsec = tv.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_sec++;
		ts->tv_nsec -= 1000000000;
	}
}

void	print_log(t_shared *shared, long long start_time,
			int coder_id, const char *action)
{
	long long	ts;

	ts = get_abs_time_ms() - start_time;
	pthread_mutex_lock(&shared->print_mutex);
	printf("%lld %d %s\n", ts, coder_id, action);
	pthread_mutex_unlock(&shared->print_mutex);
}
