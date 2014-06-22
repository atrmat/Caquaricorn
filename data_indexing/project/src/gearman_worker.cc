/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 * 
 *  Gearmand client and server library.
 *
 *  Copyright (C) 2011 Data Differential, http://datadifferential.com/
 *  Copyright (C) 2008 Brian Aker, Eric Day
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *      * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *
 *      * Redistributions in binary form must reproduce the above
 *  copyright notice, this list of conditions and the following disclaimer
 *  in the documentation and/or other materials provided with the
 *  distribution.
 *
 *      * The names of its contributors may not be used to endorse or
 *  promote products derived from this software without specific prior
 *  written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */



#include "gear_config.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#include <libgearman/gearman.h>
#include <boost/program_options.hpp>

struct reverse_worker_options_t
{
  bool chunk;
  bool status;
  bool unique;
  bool verbose;

  reverse_worker_options_t():
    chunk(false),
    status(false),
    unique(false),
    verbose(true)
  { }
};

#ifndef __INTEL_COMPILER
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

struct timeval time_start, time_end;
int64_t total_time;
FILE *worker_time;

/*
* work function: reverse the received word
*/
static gearman_return_t reverse_worker(gearman_job_st *job, void *context)
{
  reverse_worker_options_t options= *((reverse_worker_options_t *)context);

  const char *workload= (const char *)gearman_job_workload(job);
  const size_t workload_size= gearman_job_workload_size(job);

  if (options.verbose)
  {
    std::cout << "Recieved " << workload_size << " bytes" << std::endl;
  }

  std::vector<char> result;
  result.resize(workload_size);

  // Copy workload
  for (size_t y= 0, x= workload_size; x; x--, y++)
  {
    result[y]= ((uint8_t *)workload)[x - 1];
  }

  if (options.chunk) // Chunk the result set
  {
    for (size_t y= 0, x= workload_size; x; x--, y++)
    {
      if (gearman_failed(gearman_job_send_data(job, &result[y], 1)))
      {
        return GEARMAN_ERROR;
      }

      if (options.status)
      {
        // Notice that we send based on y divided by zero.
        if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workload_size)))
        {
          return GEARMAN_ERROR;
        }
      }
    }
  }
  else
  {
    if (options.status)
    {
      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)0, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
    }

    if (gearman_failed(gearman_job_send_data(job, &result[0], workload_size)))
    {
      return GEARMAN_ERROR;
    }

    if (options.status)
    {
      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)workload_size, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
    }
  }

  if (options.verbose)
  {
    std::cout << "Job=" << gearman_job_handle(job);
  }

  if (options.unique and options.verbose)
  {
    std::cout << "Unique=" << gearman_job_unique(job);
  }


  if (options.verbose)
  {
    std::cout << "  Reversed=";
    std::cout.write(workload, workload_size);
    std::cout << std::endl;
  }

  return GEARMAN_SUCCESS;
}

/*
* work function: unzip the csv files and generate the 1024 Judy files
*/
static gearman_return_t prepare_worker(gearman_job_st *job, void *context)
{
  reverse_worker_options_t options= *((reverse_worker_options_t *)context);

  const char *workload= (const char *)gearman_job_workload(job);
  const size_t workload_size= gearman_job_workload_size(job);

  if (options.verbose)
  {
    std::cout << "Recieved " << workload_size << " bytes" << std::endl;
  }

  // parse the argument_t
  std::vector<char> result;
  result.resize(workload_size);
  int prepare_begin = 0, prepare_num = 0;
  
  size_t y= 0;
  size_t x= 0;

  for (y= 0, x=0; x < workload_size; x++, y++)
  {
    result[y]= ((uint8_t *)workload)[x];
  }
  for (y = 0, x = 0; x < workload_size ;x++ )
  {
	if(result[x] == '-')
	{
		//std::cout << result[x] <<std::endl;
		for (y = 0; y < x ; y++ )
		{
			prepare_begin = prepare_begin*10 + result[y]-'0';
		}
		std::cout << prepare_begin << std::endl;
	}
	if(result[x] == ';')
	{
		for (y++; y < x ; y++ )
		{
			prepare_num = prepare_num*10 + result[y]-'0';
		}
		std::cout << prepare_num << std::endl;
	}
  }

  if (options.chunk) // Chunk the result set
  {
    for (size_t y= 0, x= workload_size; x; x--, y++)
    {
      if (gearman_failed(gearman_job_send_data(job, &result[y], 1)))
      {
        return GEARMAN_ERROR;
      }

      if (options.status)
      {
        // Notice that we send based on y divided by zero.
        if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workload_size)))
        {
          return GEARMAN_ERROR;
        }
      }
    }
  }
  else
  {
    if (options.status)
    {
      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)0, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
    }

    if (gearman_failed(gearman_job_send_data(job, &result[0], workload_size)))
    {
      return GEARMAN_ERROR;
    }

    if (options.status)
    {
      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)workload_size, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
    }
  }

  if (options.verbose)
  {
    std::cout << "PrepareJob=" << gearman_job_handle(job);
  }

  if (options.unique and options.verbose)
  {
    std::cout << "Unique=" << gearman_job_unique(job);
  }


  if (options.verbose)
  {
    std::cout << "  Reversed=";
    std::cout.write(workload, workload_size);
    std::cout << std::endl;
  }
  int thread_num = prepare_begin/prepare_num;
  std::cout<<"thread "<<thread_num<<std::endl;
  char buffer[1024];
  gettimeofday(&time_start, NULL);
  sprintf(buffer,"/root/cx_src/src/prepare_N /tmp/test/googlebooks-eng-all-5gram-20090715- %d %d /tmp/data /tmp/result/ino.txt %d",prepare_begin, prepare_num, thread_num);
  if(system(buffer) == -1){
    fprintf(stderr, "Fail to execute command: \"%s\"\n",buffer);
  }
  gettimeofday(&time_end, NULL);
  total_time = (int64_t) (time_end.tv_sec - time_start.tv_sec) * 1000000 + time_end.tv_usec - time_start.tv_usec;
  if ( (worker_time = fopen("/tmp/work_time.txt","a")) == NULL )
  {
	  fprintf(worker_time, "worker %d prepare time: %"PRId64"us", thread_num, total_time);
  }
  fclose(worker_time);
  return GEARMAN_SUCCESS;
}

/*
* work function: merge the same temp files into 1024, then fix the temp 1024 files
*/
static gearman_return_t merge_worker(gearman_job_st *job, void *context)
{
  reverse_worker_options_t options= *((reverse_worker_options_t *)context);

  const char *workload= (const char *)gearman_job_workload(job);
  const size_t workload_size= gearman_job_workload_size(job);

  if (options.verbose)
  {
    std::cout << "Recieved " << workload_size << " bytes" << std::endl;
  }

  // parse the argument_t
  std::vector<char> result;
  result.resize(workload_size);

  int merge_begin = 0, merge_end = 0;
  size_t y= 0;
  size_t x= 0;

  for (y= 0, x=0; x < workload_size; x++, y++)
  {
    result[y]= ((uint8_t *)workload)[x];
  }
  for (y = 0, x = 0; x < workload_size ;x++ )
  {
	if(result[x] == '-')
	{
		//std::cout << result[x] <<std::endl;
		for (y = 0; y < x ; y++ )
		{
			merge_begin = merge_begin*10 + result[y]-'0';
		}
		std::cout << merge_begin << std::endl;
	}
	if(result[x] == ';')
	{
		for (y++; y < x ; y++ )
		{
			merge_end = merge_end*10 + result[y]-'0';
		}
		std::cout << merge_end << std::endl;
	}
  }

  if (options.chunk) // Chunk the result set
  {
    for (size_t y= 0, x= workload_size; x; x--, y++)
    {
      if (gearman_failed(gearman_job_send_data(job, &result[y], 1)))
      {
        return GEARMAN_ERROR;
      }

      if (options.status)
      {
        // Notice that we send based on y divided by zero.
        if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workload_size)))
        {
          return GEARMAN_ERROR;
        }
      }
    }
  }
  else
  {
    if (options.status)
    {
      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)0, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
    }

    if (gearman_failed(gearman_job_send_data(job, &result[0], workload_size)))
    {
      return GEARMAN_ERROR;
    }

    if (options.status)
    {
      // Notice that we send based on y divided by zero.
      if (gearman_failed(gearman_job_send_status(job, (uint32_t)workload_size, (uint32_t)workload_size)))
      {
        return GEARMAN_ERROR;
      }
    }
  }

  if (options.verbose)
  {
    std::cout << "MergeJob=" << gearman_job_handle(job);
  }

  if (options.unique and options.verbose)
  {
    std::cout << "Unique=" << gearman_job_unique(job);
  }


  if (options.verbose)
  {
    std::cout << "  Merge=";
    std::cout.write(workload, workload_size);
    std::cout << std::endl;
  }

  char buffer[1024];
  sprintf(buffer,"/root/cx_src/src/merge_N /tmp/data /tmp/result/ino.txt /tmp/result/lno.txt /tmp/result/cdo.txt 4 %d %d",merge_begin,merge_end);

  if(system(buffer) == -1){
    fprintf(stderr, "Fail to execute command: \"%s\"\n",buffer);
  }

  return GEARMAN_SUCCESS;
}

int main(int args, char *argv[])
{
  uint64_t limit;
  reverse_worker_options_t options;
  int timeout;

  in_port_t port;
  std::string host;
  std::string identifier;
  boost::program_options::options_description desc("Options");
  desc.add_options()
    ("help", "Options related to the program.")
    ("host,h", boost::program_options::value<std::string>(&host)->default_value("localhost"),"Connect to the host")
    ("identifier", boost::program_options::value<std::string>(&identifier),"Assign identifier")
    ("port,p", boost::program_options::value<in_port_t>(&port)->default_value(GEARMAN_DEFAULT_TCP_PORT), "Port number use for connection")
    ("count,c", boost::program_options::value<uint64_t>(&limit)->default_value(0), "Number of jobs to run before exiting")
    ("timeout,u", boost::program_options::value<int>(&timeout)->default_value(-1), "Timeout in milliseconds")
    ("chunk,d", boost::program_options::bool_switch(&options.chunk)->default_value(false), "Send result back in data chunks")
    ("status,s", boost::program_options::bool_switch(&options.status)->default_value(false), "Send status updates and sleep while running job")
    ("unique,u", boost::program_options::bool_switch(&options.unique)->default_value(false), "When grabbing jobs, grab the uniqie id")
    ("verbose", boost::program_options::bool_switch(&options.verbose)->default_value(true), "Print to stdout information as job is processed.")
            ;

  boost::program_options::variables_map vm;
  try
  {
    boost::program_options::store(boost::program_options::parse_command_line(args, argv, desc), vm);
    boost::program_options::notify(vm);
  }
  catch(std::exception &e)
  { 
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  if (vm.count("help"))
  {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    std::cerr << "signal:" << strerror(errno) << std::endl;
    return EXIT_FAILURE;
  }

  gearman_worker_st *worker;
  if ((worker= gearman_worker_create(NULL)) == NULL)
  {
    std::cerr << "Memory allocation failure on worker creation." << std::endl;
    return EXIT_FAILURE;
  }

  if (options.unique)
  {
    gearman_worker_add_options(worker, GEARMAN_WORKER_GRAB_UNIQ);
  }

  if (timeout >= 0)
  {
    gearman_worker_set_timeout(worker, timeout);
  }

  if (gearman_failed(gearman_worker_add_server(worker, host.c_str(), port)))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }

  if (identifier.empty() == false)
  {
    if (gearman_failed(gearman_worker_set_identifier(worker, identifier.c_str(), identifier.size())))
    {
      std::cerr << gearman_worker_error(worker) << std::endl;
      return EXIT_FAILURE;
    }
  }
  // Add a work function
  // reverse work
  gearman_function_t worker_fn= gearman_function_create(reverse_worker);
  if (gearman_failed(gearman_worker_define_function(worker,
                                                    gearman_literal_param("reverse"),
                                                    worker_fn,
                                                    0, 
                                                    &options)))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }
  //prepare work
  gearman_function_t worker_fn0= gearman_function_create(prepare_worker);
  if (gearman_failed(gearman_worker_define_function(worker,
                                                    gearman_literal_param("prepare_N"),
                                                    worker_fn0,
                                                    0, 
                                                    &options)))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }
  //merge work
  gearman_function_t worker_fn1= gearman_function_create(merge_worker);
  if (gearman_failed(gearman_worker_define_function(worker,
                                                    gearman_literal_param("merge_N"),
                                                    worker_fn1,
                                                    0, 
                                                    &options)))
  {
    std::cerr << gearman_worker_error(worker) << std::endl;
    return EXIT_FAILURE;
  }

  // Add one if count is not zero
  if (limit != 0)
  {
    limit++;
  }

  while (--limit)
  {
    if (gearman_failed(gearman_worker_work(worker)))
    {
      std::cerr << gearman_worker_error(worker) << std::endl;
      break;
    }
  }

  gearman_worker_free(worker);

  return EXIT_SUCCESS;
}
