/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Signal.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asalo <asalo@student.hive.fi>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/01 13:13:09 by asalo             #+#    #+#             */
/*   Updated: 2025/02/03 10:27:55 by asalo            ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <csignal>
#include <iostream>
#include <atomic>

// Global flag to indicate shutdown request.
volatile std::sig_atomic_t g_shutdownRequested = 0;

// Signal handler function
void signalHandler(int signal) {
    if (signal == SIGINT || signal == SIGTERM) {
        g_shutdownRequested = 1;
    }
}

