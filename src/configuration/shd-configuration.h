/*
 * The Shadow Simulator
 *
 * Copyright (c) 2010-2011 Rob Jansen <jansen@cs.umn.edu>
 *
 * This file is part of Shadow.
 *
 * Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHD_CONFIGURATION_H_
#define SHD_CONFIGURATION_H_

#include <glib.h>

/**
 * @addtogroup Configuration
 * @{
 * Use this module to parse command line input.
 */

/**
 * Simulation time in nanoseconds. Allows for a consistent representation
 * of time throughput the simulator.
 */
typedef guint64 SimulationTime;

/**
 * Represents an invalid simulation time.
 */
#define SIMTIME_INVALID G_MAXUINT64

/**
 * Represents one nanosecond in simulation time.
 */
#define SIMTIME_ONE_NANOSECOND G_GUINT64_CONSTANT(1)

/**
 * Represents one microsecond in simulation time.
 */
#define SIMTIME_ONE_MICROSECOND G_GUINT64_CONSTANT(1000)

/**
 * Represents one millisecond in simulation time.
 */
#define SIMTIME_ONE_MILLISECOND G_GUINT64_CONSTANT(1000000)

/**
 * Represents one second in simulation time.
 */
#define SIMTIME_ONE_SECOND G_GUINT64_CONSTANT(1000000000)

/**
 * Represents one minute in simulation time.
 */
#define SIMTIME_ONE_MINUTE G_GUINT64_CONSTANT(60000000000)

/**
 * Represents one hour in simulation time.
 */
#define SIMTIME_ONE_HOUR G_GUINT64_CONSTANT(3600000000000)

#if 1
/**
 * Memory magic for assertions that memory has not been freed. The idea behind
 * this approach is to declare a value in each struct using MAGIC_DECLARE,
 * define it using MAGIC_INIT during object creation, and clear it during
 * cleanup using MAGIC_CLEAR. Any time the object is referenced, we can check
 * the magic value using MAGIC_ASSERT. If the assert fails, there is a bug.
 *
 * In general, this should only be used in DEBUG mode. Once we are somewhat
 * convinced on Shadow's stability (for releases), these macros will do nothing.
 *
 * MAGIC_VALUE is an arbitrary value.
 *
 * @todo add #ifdef DEBUG
 */
#define MAGIC_VALUE 0xAABBCCDD

/**
 * Declare a member of a struct to hold a MAGIC_VALUE. This should be placed in
 * the declaration of a struct, generally as the last member of the struct.
 */
#define MAGIC_DECLARE guint magic

/**
 * Initialize a value declared with MAGIC_DECLARE to MAGIC_VALUE
 */
#define MAGIC_INIT(object) object->magic = MAGIC_VALUE

/**
 * Assert that a struct declared with MAGIC_DECLARE and initialized with
 * MAGIC_INIT still holds the value MAGIC_VALUE.
 */
#define MAGIC_ASSERT(object) g_assert(object && (object->magic == MAGIC_VALUE))

/**
 * CLear a magic value. Future assertions with MAGIC_ASSERT will fail.
 */
#define MAGIC_CLEAR(object) object->magic = 0
#else
#define MAGIC_VALUE
#define MAGIC_DECLARE
#define MAGIC_INIT(object)
#define MAGIC_ASSERT(object)
#define MAGIC_CLEAR(object)
#endif

/**
 * We intercept read, write, and close calls since they may be done on our
 * virtual sockets. However, applications may also want to read/write/close a
 * regular file. We differentiate these by handing out high descriptors for
 * our virtual sockets. Any descriptor below this cutoff can be considered a
 * real file.
 *
 * It is important to set this high enough so in large simulations the system
 * file descriptor counter doesnt collide with our sockets. So this should be
 * set over the ulimit -n value.
 *
 * @todo FIXME we should implement socket descriptors greater than ugint16 so we can use
 * all 31 bits of the gint!
 */
#define VNETWORK_MIN_SD 30000

/**
 * A shortcut for turning an IP address in network format to a string in
 * dot-and-number format. The returned string is returned in a statically
 * allocated buffer and so is not thread-safe.
 *
 * @todo FIXME this needs to change as its not thread safe and leaks memory
 */
#define NTOA(ip) inet_ntoa((struct in_addr){ip})

/**
 * We always use TCP_autotuning unless this is set to TRUE
 *
 * @todo change this to a command line option accessible via #Configuration
 */
#define CONFIG_SEND_BUFFER_SIZE_FORCE FALSE

/**
 * Default size of the send buffer per socket if TCP-autotuning is not used.
 * This value was computed from "man tcp"
 *
 * @todo change this to a command line option accessible via #Configuration
 */
#define CONFIG_SEND_BUFFER_SIZE 131072

/**
 * Default size of the receive buffer per socket if TCP-autotuning is not used
 * This value was computed from "man tcp"
 *
 * @todo change this to a command line option accessible via #Configuration
 */
#define CONFIG_RECV_BUFFER_SIZE 174760

/**
 * TRUE if we should do delayed acknowledgments, FALSE otherwise
 *
 * @todo change this to a command line option accessible via #Configuration
 */
#define CONFIG_DO_DELAYED_ACKS FALSE

typedef struct _Configuration Configuration;

struct _Configuration {
	GOptionContext *context;

	GOptionGroup* mainOptionGroup;
	gchar* logLevelInput;
	gint nWorkerThreads;
	gboolean printSoftwareVersion;

	GOptionGroup* networkOptionGroup;
	gint minRunAhead;

	GOptionGroup* pluginsOptionGroup;
	gboolean runPingExample;
	gboolean runEchoExample;
	gboolean runFileExample;

	GQueue* inputXMLFilenames;

	MAGIC_DECLARE;
};

/**
 * Create a new #Configuration and parse the command line arguments given in
 * argv. Errors encountered during parsing are printed to stderr.
 *
 * @param argc the length of the argument vector in argv
 * @param argv a vector of arguments passed in from the command line while
 * launching Shadow
 *
 * @returns a new #Configuration which should be freed with configuration_free(),
 * or NULL if an error was encountered
 */
Configuration* configuration_new(gint argc, gchar* argv[]);

/**
 * Frees a previously created #Configuration
 *
 * @param config a #Configuration object created with configuration_new()
 */
void configuration_free(Configuration* config);

/**
 * Get the configured log level based on command line input.
 *
 * @param config a #Configuration object created with configuration_new()
 *
 * @returns the log level as parsed from command line input
 */
GLogLevelFlags configuration_getLogLevel(Configuration* config);

/** @} */

#endif /* SHD_CONFIGURATION_H_ */