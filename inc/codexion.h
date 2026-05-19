/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pmelo-cl <pmelo-cl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:33:23 by pmelo-cl          #+#    #+#             */
/*   Updated: 2026/05/19 17:59:21 by pmelo-cl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <errno.h>

# define SCHED_FIFO_TYPE 0
# define SCHED_EDF_TYPE  1

# define STATE_IDLE        0
# define STATE_COMPILING   1
# define STATE_DEBUGGING   2
# define STATE_REFACTORING 3

typedef struct s_config
{
	int			num_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			compiles_required;
	int			dongle_cooldown;
	int			scheduler;
	long long	start_time;
}	t_config;

typedef struct s_wait_node
{
	int					coder_id;
	long long			deadline;
	long long			arrival_time;
	struct s_wait_node	*next;
}	t_wait_node;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				occupied;
	long long		cooldown_end;
	t_wait_node		*wait_head;
	t_wait_node		*wait_tail;
}	t_dongle;

typedef struct s_shared
{
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	running_mutex;
	int				running;
}	t_shared;

typedef struct s_coder
{
	int				id;
	int				state;
	int				compiles_done;
	long long		last_compile_start;
	long long		deadline;
	pthread_mutex_t	state_mutex;
	pthread_t		thread;
	t_dongle		*dongle_left;
	t_dongle		*dongle_right;
	t_config		*cfg;
	t_shared		*shared;
	t_dongle		*all_dongles;
}	t_coder;

typedef struct s_monitor
{
	pthread_t		thread;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	t_coder			*coders;
	t_dongle		*dongles;
	t_config		*cfg;
	t_shared		*shared;
}	t_monitor;

typedef struct s_sim
{
	t_config	cfg;
	t_shared	shared;
	t_dongle	*dongles;
	t_coder		*coders;
	t_monitor	mon;
}	t_sim;

int			parse_args(int argc, char **argv, t_config *cfg);
int			init_simulation(t_sim *sim);
int			start_simulation(t_sim *sim);
void		cleanup_simulation(t_sim *sim, int created);

long long	get_abs_time_ms(void);
void		ms_sleep(int ms);
void		get_abs_timespec(struct timespec *ts, long long timeout_ms);
void		print_log(t_shared *shared, long long start_time,
				int coder_id, const char *action);

void		enqueue_wait(t_dongle *d, t_wait_node *node, int scheduler);
t_wait_node	*dequeue_wait(t_dongle *d);
void		remove_wait_node(t_dongle *d, t_wait_node *node);
void		enqueue_edf(t_dongle *d, t_wait_node *node);

int			init_dongles(t_dongle **dongles, t_config *cfg);
void		destroy_dongles(t_dongle *dongles, int count);
int			dongle_take(t_dongle *d, t_coder *coder);
void		dongle_release(t_dongle *d, t_config *cfg, t_shared *shared,
				int coder_id);
void		dongle_release(t_dongle *d, t_config *cfg, t_shared *shared,
				int coder_id);
int			dongle_take(t_dongle *d, t_coder *coder);

int			init_coders(t_coder **coders, t_dongle *dongles,
				t_config *cfg, t_shared *shared);
void		destroy_coders(t_coder *coders, int count);
void		*coder_routine(void *arg);

int			init_monitor(t_monitor *mon, t_sim *sim);
void		destroy_monitor(t_monitor *mon);
void		*monitor_routine(void *arg);

int			check_burnout(t_monitor *mon);
long long	next_deadline(t_monitor *mon);
int			all_completed(t_monitor *mon);

void		stop_simulation(t_monitor *mon);

int			check_running(t_monitor *mon);
int			handle_completion(t_monitor *mon);
void		wait_next_deadline(t_monitor *mon);
int			handle_burnout(t_monitor *mon);

#endif