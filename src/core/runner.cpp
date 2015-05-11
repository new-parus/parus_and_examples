/*
 *  This file is a part of the PARUS project.
 *  Copyright (C) 2006  Alexey N. Salnikov (salnikov@cmc.msu.ru)
 *  
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* If glibc is not supported using local getopt */
#ifdef _GNU_SOURCE
 #include <getopt.h>
#else
 #include <unistd.h>
#endif


#include <parus_config.h>

#include "px_const.h"
#include "edge_memory.h"
#include "px_log_print.h"
#include "my_time.h"
#include "my_malloc.h"
#include "network_speed.h"



int px_mpi_comm_size;
int px_mpi_comm_rank;

char px_host_name[256];

PX_Edge_data_memory px_edges_data;


extern int px_argc;
extern char **px_argv;

/*
 * The body of this function is generated by graph2c++ utility 
 * automatically. 
 */
extern int px_runner(int px_node);

extern int px_daemon
(
	char *gr_name,
	char *sch_name,
	char *v_name,
	char *mtr_name,
	char *px_daemon_work_regime
);

int main(int argc,char **argv)
{
	MPI_Status px_mpi_status;
	px_my_time_type time_beg,time_end;
	
	FILE *f=NULL;
	
	int px_flag;
	int px_regime;
	int px_node;
	int px_message;
	
	int i;
	int tmp;
	char str[400];

	int px_edge_send_header[2];
	
	char *px_graph="graph.grf";
	char *px_schedule="sch.grf";
	char *px_procs=NULL;
	char *px_net=NULL;
	char *px_daemon_work_regime="dynamic";
	char *px_arg_debug_level="0";
	
	char px_error_flag=0;
	char px_version_flag=0;
	char px_help_flag=0;
	int px_arg_val;
	int px_arg_index=0;


#ifdef _GNU_SOURCE
	struct option px_options[10]=
	{
		{"graph",required_argument,NULL,'g'},
		{"schedule",required_argument,NULL,'s'},
		{"procs",required_argument,NULL,'p'},
		{"net",required_argument,NULL,'n'},
		{"regime",required_argument,NULL,'r'},
		{"debug",required_argument,NULL,'d'},
		{"version",no_argument,NULL,'v'},
		{"help",no_argument,NULL,'h'},
		{0,0,0,0}
	};               
	
#endif
 	MPI_Datatype struct_types[4]= {MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE,MPI_DOUBLE};
    MPI_Datatype MPI_My_time_struct;
    int blocklength[4]= {1,1,1,1/*,1*/};
    MPI_Aint displace[4],base;
 	
 	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&px_mpi_comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD,&px_mpi_comm_rank);
	
	/*
	 *
	 * Reading command line options
	 *
	 */
	while(1)
	{
#ifdef _GNU_SOURCE
		px_arg_val=getopt_long(argc,argv,"g:s:p:n:r:d:h:v",px_options,NULL);
#else
		px_arg_val=getopt(argc,argv,"g:s:p:n:r:d:h:v");
#endif
		if(px_arg_val==-1)
		{
			break;
		}
		
		px_arg_index++;
		
		switch(px_arg_val)
		{
			case 'g':
				px_graph=optarg;
			break;
			case 's':
				px_schedule=optarg;
			break;
			case 'p':
				px_procs=optarg;
			break;
			case 'n':
				px_net=optarg;
			break;
			case 'r':
				px_daemon_work_regime=optarg;
			break;
			case 'd':
				px_arg_debug_level=optarg;
			break;
			case 'v':
				px_version_flag=1;
			break;
			case 'h':
				px_help_flag=1;
			break;
			case '?':
				px_error_flag=1;
			break;
		} /* end switch */
		
		if(px_error_flag)
		{
			break;
		}
	} /* end while */
	
	if(px_mpi_comm_rank==0)
	{
		/*
		 * printing version
		 */
		if(px_version_flag)
		{
			printf("%s\n",PARUS_VERSION);
			MPI_Barrier(MPI_COMM_WORLD);
			px_regime=PX_STOP;
			
			for(i=1;i<px_mpi_comm_size;i++)
			{
				MPI_Send(&px_regime,1,MPI_INT,i,PX_REGIME_TAG,MPI_COMM_WORLD);
			}
			
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Finalize();
			return 0;
		}
		
		if(px_mpi_comm_size==1)
		{
			fprintf
			(
				stderr,
				"\n\nThe program needs more than one prosessors to run itself.\n"
			);
			px_error_flag=1;
		}
		
		if((px_error_flag)||(px_help_flag))
		{

#ifdef _GNU_SOURCE
			printf("\nCommand line format for this program (not for mpirun) is:\n");
			printf("%s\t\t[{ -g | --graph } <graph file> ]\n",argv[0]);
			printf("\t\t\t[{ -s | --schedule } <schedule file> ]\n");
			printf("\t\t\t[{ -p | --procs } <speed of processors file> ]\n");
			printf("\t\t\t[{ -n | --net } <speed of network file> ]\n");
			printf("\t\t\t[{ -r | --regime } {dynamic|static|combined}]\n");
			printf("\t\t\t[{ -d | --debug } <digit value>]\n");
			printf("\t\t\t[{ -h | --help }]\n");
			printf("\t\t\t[{ -v | --version }]\n");
#else
			printf("\nCommand line format for this program (not for mpirun) is:\n");
			printf("%s\t\t[ -g <graph file> ]\n",argv[0]);
			printf("\t\t\t[ -s <schedule file> ]\n");
			printf("\t\t\t[ -p <speed of processors file> ]\n");
			printf("\t\t\t[ -n <speed of network file> ]\n");
			printf("\t\t\t[ -r {dynamic|static|combined}]\n");
			printf("\t\t\t[ -d debug <digit value>]\n");
			printf("\t\t\t[ -h ] - print help\n");
			printf("\t\t\t[ -v ] - print version\n");
#endif   
			printf("\t\t\t[ -- [<arguments of user's program>]]\n");
			
			printf("\n\nDefault values are:\n");
			printf("graph\t\t - ./graph.grf\n");
			printf("schedule\t - ./sch.grf\n");
			printf("procs\t\t - %s/etc/procs.vct\n",PARUS_INSTALL_DIR);
			printf("net\t\t - %s/etc/network.average\n",PARUS_INSTALL_DIR);
			printf("regime\t\t - dynamic\n");
			printf("debug\t\t - 0\n");
			printf("help\t\t - this text\n");
			printf("version\t\t - types parus version\n\n\n");
			printf("Parus version: %s\n\n\n",PARUS_VERSION);
			
			MPI_Barrier(MPI_COMM_WORLD);
			px_regime=PX_STOP;
			for(i=1;i<px_mpi_comm_size;i++)
			{
				MPI_Send(&px_regime,1,MPI_INT,i,PX_REGIME_TAG,MPI_COMM_WORLD);
			}
			
			MPI_Barrier(MPI_COMM_WORLD);
			MPI_Finalize();
			
			if(px_error_flag)
			{
				return -1;
			}
			return 0;
		} /* End if error_flag and  help_flag */
		

		/*
		 *
		 * Set default file name for px_procs
		 *
		 */
		if(px_procs==NULL)
		{
			px_procs=(char *)malloc
			(
				(strlen(PARUS_INSTALL_DIR)+
				strlen("/etc/procs.vct")+1)*
				sizeof(char)
			);
			
		
			if(px_procs==NULL)
			{
				px_log_printf(0,"Can't allocate memmory for px_procs file name\n");
				fprintf(stderr,"Can't allocate memmory for px_procs file name\n");
				MPI_Abort(MPI_COMM_WORLD,-1);
				return -1;
			}
			
			sprintf(px_procs,"%s/etc/procs.vct",PARUS_INSTALL_DIR);
		}
		
		
		/*
		 *
		 * Set default file name for px_net 
		 *
		 */
		if(px_net==NULL)
		{
			px_net=(char *)malloc
			(
				(strlen(PARUS_INSTALL_DIR)+
				strlen("/etc/network.average")+1)*
				sizeof(char)
			);
			
			if(px_net==NULL)
			{
				px_log_printf(0,"Can't allocate memmory for px_net file name\n");
				fprintf(stderr,"Can't allocate memmory for px_net file name\n");
				MPI_Abort(MPI_COMM_WORLD,-1);
				return -1;
			}
			
			sprintf(px_net,"%s/etc/network.average",PARUS_INSTALL_DIR);
		}
		
		/*
		 *
		 * Counting 0 point of time!
		 *
		 */
		time_beg=px_my_cpu_time();
	} /* End parse parameters by proc with number 0 */ 
	
	/*
	 * 
	 * Initialization log
	 * 
	 */
	sprintf(str,"%s_%d",argv[0],px_mpi_comm_rank);
	px_log_init(atoi(px_arg_debug_level),str,"w");
	
	/*
	 * 
	 * Send comand line parameters
	 * 
	 */
	px_argc=argc-optind+1;
	MPI_Bcast(&px_argc,1,MPI_INT,0,MPI_COMM_WORLD);
	
	px_argv=(char **)malloc((px_argc)*sizeof(char *));
 
	if(px_argv==NULL)
	{
		px_log_printf
		(
			0,
			"Memory allocation error! Can't allocate memory for command line parameters\n\n"
		);
		
		fprintf
		(
			stderr,
			"Memory allocation error! Can't allocate memory for command line parameters\n\n"
		);
		
		MPI_Abort(MPI_COMM_WORLD,-1);
		return -1;
	}
	
	if(px_mpi_comm_rank==0)
	{
		px_argv[0]=(char *)malloc((strlen(argv[0])+1)*sizeof(char));
		if(px_argv[0]==NULL)
		{
			px_log_printf(0,"Memory allocation error! Can't allocate memory for command line parameter 0\n\n");
			fprintf(stderr,"Memory allocation error! Can't allocate memory for command line parametr 0\n\n");
			free(px_argv);
			MPI_Abort(MPI_COMM_WORLD,-1);
			return -1;
		}
		
		strcpy(px_argv[0],argv[0]);
		
		for(i=1;i<px_argc;i++)
		{
			px_argv[i]=(char *)malloc
			(
				(strlen(argv[i-1+optind])+1)*
				sizeof(char)
			);

			if(px_argv[i]==NULL)
			{
				px_log_printf(0,"Memory allocation error! Can't allocate memory for command line parametr %d\n\n",i);
				fprintf(stderr,"Memory allocation error! Can't allocate memory for command line parametrs %d\n\n",i);
				MPI_Abort(MPI_COMM_WORLD,-1);
				return -1;
			}
			
			strcpy(px_argv[i],argv[i-1+optind]);
		}
	} /*end parse program parameters in MPI-process 0 */   
 
	/*
	 *
	 * Send arguments for all MPI processes
	 *
	 */
	for(i=0;i<px_argc;i++)
	{
		if(px_mpi_comm_rank==0)
		{
			tmp=strlen(px_argv[i])+1;
		}
		
		MPI_Bcast(&tmp,1,MPI_INT,0,MPI_COMM_WORLD);
		
		if(px_mpi_comm_rank>0)
		{
			px_argv[i]=(char *)malloc(tmp*sizeof(char));
			if(px_argv[i]==NULL)
			{
				px_log_printf(0,"Memory allocation error! Can't allocate memory for command line parametr after MPI_Bcast\n\n");
				fprintf(stderr,"Memory allocation error! Can't allocate memory for command line parametr after MPI_Bcast\n\n");
				MPI_Abort(MPI_COMM_WORLD,-1);
				return -1;
			}
		}
		MPI_Bcast(px_argv[i],tmp,MPI_CHAR,0,MPI_COMM_WORLD);
	}
	
	/*
	 *
	 * Get Hostname for MPI-process
	 *
	 */
	gethostname(px_host_name,255);
	px_log_printf
	(
 		1,
		"Process %d from %d already ran and has hostname '%s'\n\n\n",
		px_mpi_comm_rank,
		px_mpi_comm_size,
		px_host_name
	);

	
	/*
	 *
	 * Printing parameters 
	 *
	 */
	if(px_mpi_comm_rank==0)
	{
		px_log_printf
		(
			0,
			"The program '%s' is running on %d processors with next parameters:\n\n",
			argv[0],
			px_mpi_comm_size
		);
		px_log_printf(0,"\t\tGraph file name '%s'\n",px_graph);
		px_log_printf(0,"\t\tSchedule file name '%s'\n",px_schedule);
		px_log_printf(0,"\t\tProcesorces speed file name '%s'\n",px_procs);
		px_log_printf(0,"\t\tNetwork speed file name '%s'\n",px_net);
		px_log_printf(0,"\t\tScheduling regime is '%s'\n\n",px_daemon_work_regime);
	}
	
	
	/*
	 * 
	 * Root node execution
	 *
	 */
	px_log_printf
	(
		1,
		"The \"root\" is running on the process %d from %d\n",
		px_mpi_comm_rank,
		px_mpi_comm_size
	);	
	px_flag=px_runner(PX_ROOT_NODE);
	
	px_log_printf
	(
		1,
		"The \"root\" is finished with status %d\n",
		px_flag
	);
	

	/*
	 *
	 * Error in root node!
	 *
	 */
	if(px_flag==-1)
	{
		px_log_printf(0,"Error in root node! Parallel programm will be terminated\n");
		fprintf(stderr,"Error in root node! Parallel programm will be terminated\n");
		
		for(i=0;i<px_argc;i++)
		{
			if(px_argv[i]!=NULL)
			{
				free(px_argv[i]);
				px_argv[i]=NULL;
			}
		}
		
		if(px_argv!=NULL)
		{
			free(px_argv);
			px_argv=NULL;
		}
		
		MPI_Abort(MPI_COMM_WORLD,-1);
		return -1;
	}
	
 	MPI_Barrier(MPI_COMM_WORLD);
	
	/*
	 * 
	 * end root node execution
	 * 
	 */
	
	/*
	 *
	 * Daemon execution
	 *
	 */
	if(px_mpi_comm_rank==0)
	{
		px_log_printf
		(
			1,
			"process %d from %d: it executes daemon now\n",
			px_mpi_comm_rank,
			px_mpi_comm_size
		);
		
		px_flag=px_daemon
		(
			px_graph,
			px_schedule,
			px_procs,
			px_net,
			px_daemon_work_regime
		);
		
		px_log_printf
		(
			1,
			"process %d from %d: the daemon is finished with status %d\n",
			px_mpi_comm_rank,
			px_mpi_comm_size,
			px_flag
		);
		
		/*
		 *
		 * After finishing daemon we need to finish 
		 * other MPI-processes.
		 *
		 */
		px_regime=PX_STOP;
		
		for(i=1;i<px_mpi_comm_size;i++)
		{
			px_proc_regime_to_string(px_regime,str);
			px_log_printf
			(
				2,
				"before send PX_REGIME ( \"%s\" ) to processor %d\n",
				str,
				i
			);
			
			MPI_Send(&px_regime,1,MPI_INT,i,PX_REGIME_TAG,MPI_COMM_WORLD);
			
			px_log_printf
			(
				2,
				"after  send PX_REGIME ( \"%s\" ) to processor %d\n",
				str,
				i
			);
		} /* End for */
		
		time_end=px_my_cpu_time();
		
		f=fopen("times.txt","a");
		if(f==NULL)
		{
			fprintf(stderr,"Can't open file \"times.txt\".\n");
			px_log_printf(0,"Can't open file \"times.txt\".\n");
			px_log_close();
			MPI_Finalize();
			return -1;
		}
		
		fprintf
		(
			f,
			"Work time '%s' on the %d processors is %f\n",
			argv[0],
			px_mpi_comm_size,
			(double)(time_end-time_beg)
		);
		fclose(f);
		
		printf
		(
			"PARUS: Work time '%s' on the %d processors is %f\n",
			argv[0],
			px_mpi_comm_size,
			(double)(time_end-time_beg)
		);
		
	} /* End 0 proc code (end daemon execution) */
	else
	/*
	 * Code for other MPI-processes
	 *
	 */
	{
		px_log_printf
		(
			1,
			"process %d from %d is running\n",
			px_mpi_comm_rank,
			px_mpi_comm_size
		);

		/*
		 * Data for network test 
		 */
		char* mtr_name = "file";
		Network_speed mtr;
		mtr.make_file(mtr_name);
		
		px_message=PX_PROCESSOR_CLEAN;
		
		MPI_Send(&px_message,1,MPI_INT,0,PX_NODE_CLEAN_TAG,MPI_COMM_WORLD);
		px_log_printf
		(
			2,
			"process %d from %d before recv PX_REGIME\n",
			px_mpi_comm_rank,
			px_mpi_comm_size
		);
		
		MPI_Recv
		(
			&px_regime,
			1,
			MPI_INT,
			0,
			PX_REGIME_TAG,
			MPI_COMM_WORLD,
			&px_mpi_status
		);
		
		px_proc_regime_to_string(px_regime,str);
		px_log_printf
		(
			2,
			"process %d from %d after  recv PX_REGIME ( \"%s\" )\n",
			px_mpi_comm_rank,
			px_mpi_comm_size,
			str
		);
		
		while(px_regime!=PX_STOP)
		{
			/*
			 *
			 * Testing of the data transfers which are 
			 * finished.
			 *
			 */
			px_log_printf(2,"before edges_data_test\n");
			px_flag=px_edges_data.test();
			px_log_printf(2,"after  edges_data_test\n");
			
			/*
			 *
			 * Sending edges
			 *
			 */
			if(px_regime==PX_EDGE_REGIME)
			{
				px_log_printf(2,"before recv PX_EDGES_SEND_TAG\n");
				MPI_Recv
				(
					&px_edge_send_header,
					2,
					MPI_INT,
					0,
					PX_EDGES_SEND_TAG,
					MPI_COMM_WORLD,
					&px_mpi_status
				);				
				px_log_printf(2,"after  recv PX_EDGES_SEND_TAG\n");
				
				px_log_printf
				(
					2,
					"before px_edges_data.send(edge=%d,processor=%d)\n",
					px_edge_send_header[0],
					px_edge_send_header[1]
				);
				
				px_flag=px_edges_data.send
				(
					px_edge_send_header[0], // edge number
					px_edge_send_header[1]  // to processor
				);
				
				px_log_printf
				(
					2,
					"after  px_edges_data.send(edge=%d,processor=%d)\n",
					px_edge_send_header[0],
					px_edge_send_header[1]
				);
				
				if(px_flag==-1)
				{
					fprintf
					(
						stderr,
						"The process %d from %d produced an error.\n",
						px_mpi_comm_rank,
						px_mpi_comm_size
					);
					
					px_log_printf
					(
						0,
						"The process %d from %d produced an error.\n",
						px_mpi_comm_rank,
						px_mpi_comm_size
					);
					
					fprintf
					(
						stderr,
						"The error appears after process tried to send"
						" edge %d to the processor %d\n",
						px_edge_send_header[0],
						px_edge_send_header[1]
					);
					
					px_log_printf
					(
						0,
						"The error appears after process can't to send"
						" edge %d to the processor %d\n",
						px_edge_send_header[0],
						px_edge_send_header[1]
					);
					
					for(i=0;i<px_argc;i++)
					{
						if(px_argv[i]!=NULL)
						{
							free(px_argv[i]);
							px_argv[i]=NULL;
						}
					}
					
					if(px_argv!=NULL)
					{
						free(px_argv);
						px_argv=NULL;
					}
					
					MPI_Abort(MPI_COMM_WORLD,-1);
					return -1;
				}
			}
			

			/*
			 *
			 * Node executing
			 *
			 */
			if(px_regime==PX_WORK)
			{
				px_log_printf
				(
					2,
					"process %d from %d: before recv PX_NODE_QUESTION\n",
					px_mpi_comm_rank,
					px_mpi_comm_size
				);
				
				MPI_Recv
				(
					&px_node,
					1,
					MPI_INT,
					0,
					PX_NODE_QUESTION_TAG,
					MPI_COMM_WORLD,
					&px_mpi_status
				);
				
				px_log_printf
				(
					2,
					"process %d from %d: after  recv PX_NODE_QUESTION NODE=%d\n",
					px_mpi_comm_rank,
					px_mpi_comm_size,
					px_node
				);
				
				px_log_printf
				(
					1,
					"process %d from %d: it is runnig the node %d\n",
					px_mpi_comm_rank,
					px_mpi_comm_size,
					px_node
				);
				
				px_flag=px_runner(px_node);
				if(px_flag==-1)
				{
					fprintf
					(
						stderr,
						"process %d from %d:\n",
						px_mpi_comm_rank,
						px_mpi_comm_size
					);
					
					fprintf
					(
						stderr,
						"Error appears during the node %d execution\n",
						px_node
					);
					
					px_log_printf
					(
						0,
						"process %d from %d: ",
						px_mpi_comm_rank,
						px_mpi_comm_size
					);
					
					px_log_printf
					(
						0,
						"an error appears during the node %d execution\n",
						px_node
					);
				
					for(i=0;i<px_argc;i++)
					{
						if(px_argv[i]!=NULL)
						{
							free(px_argv[i]);
							px_argv[i]=NULL;
						}
					}
					
					if(px_argv!=NULL)
					{
						free(px_argv);
						px_argv=NULL;
					}
					
					MPI_Abort(MPI_COMM_WORLD,-1);
					return -1;
				}
				
				px_log_printf
				(
					1,
					"Node %d is finished on the %d with status %d\n",
					px_node,
					px_mpi_comm_rank,
					px_flag
				);
				
				px_message=PX_PROCESSOR_CLEAN;
				
				px_log_printf
				(
					2,
					"process %d from %d  before send PX_NODE_CLEAN\n",
					px_mpi_comm_rank,
					px_mpi_comm_size
				);
				
				MPI_Send
				(
					&px_message,
					1,
					MPI_INT,
					0,
					PX_NODE_CLEAN_TAG,
					MPI_COMM_WORLD
				);
				
				px_log_printf
				(
					2,
					"process %d from %d  after  send PX_NODE_CLEAN\n",
					px_mpi_comm_rank,
					px_mpi_comm_size
				);
			} /* End node execution */

			
			/*
			 *
			 * Reading next command from the daemon
			 *
			 */
			px_log_printf
			(
				2,
				"process %d from %d before recv PX_REGIME\n",
				px_mpi_comm_rank,
				px_mpi_comm_size
			);
			
			MPI_Recv
			(
				&px_regime,
				1,
				MPI_INT,
				0,
				PX_REGIME_TAG,
				MPI_COMM_WORLD,
				&px_mpi_status
			);
			
			px_proc_regime_to_string(px_regime,str);
			px_log_printf
			(
				2,
				"process %d from %d after  recv PX_REGIME ( \"%s\" )\n",
				px_mpi_comm_rank,
				px_mpi_comm_size,
				str
			);
		} /* End while(px_regime!=px_stop) */
	} /* End code for MPI-processes which number is not zero */
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	
	/*
	 * 
	 * Tail node execution
	 *
	 */
	px_log_printf
	(
		1,
		"The \"tail\" is running on the process %d from %d\n",
		px_mpi_comm_rank,
		px_mpi_comm_size
	);	
	px_flag=px_runner(PX_TAIL_NODE);
	
	px_log_printf
	(
		1,
		"The \"tail\" is finished with status %d\n",
		px_flag
	);

	
	/*
	 *
	 * Finishing actions.
	 *
	 */
	px_log_printf
	(
		1,
		"process %d from %d is finished\n",
		px_mpi_comm_rank,
		px_mpi_comm_size
	);
	
	px_log_close();
 	for(i=0;i<px_argc;i++)
	{
		if(px_argv[i]!=NULL)
		{
			free(px_argv[i]);
			px_argv[i]=NULL;
		}
	}
					
	if(px_argv!=NULL)
	{
		free(px_argv);
		px_argv=NULL;
	}
	
	MPI_Finalize();
	return 0;
}

