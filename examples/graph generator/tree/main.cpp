#include <iostream>
#include <fstream>

using namespace std;

ofstream node;
ofstream edge;
int number_of_nodes;

void printnode(int number, int from, int to)
{
	int i=1, layer=0;
	while (i<number+1) 
	{
		i=i*2;
		layer+=1;
	}
	node<<"    <NODE_BEGIN>"<<endl;
    node<<"        number "<<number<<endl;
    node<<"        type 1"<<endl;
    node<<"        weight 100"<<endl;
    node<<"        layer "<<layer<<endl;
    node<<"        num_input_edges 1"<<endl;
    node<<"        edges ( "<<number<<" )"<<endl;
    if (to==0)
    {
    	node<<"        num_output_edges 0"<<endl;
    	node<<"        edges (  )"<<endl;
    }else{
		node<<"        num_output_edges 2"<<endl;
    	node<<"        edges ( "<<to<<" "<<to+1<<" )"<<endl;
    }
    node<<"        head \"head\""<<endl;
    node<<"        body \"node\""<<endl;
    node<<"        tail \"\""<<endl;
    node<<"    <NODE_END>"<<endl<<endl;
}

void printedge(int number, int from, int to)
{
    edge<<"    <EDGE_BEGIN>"<<endl;
    edge<<"       number "<<number<<endl;
    edge<<"       weight 100"<<endl;
    edge<<"       type   GRAPH_NONE"<<endl;
    edge<<"       num_var 8"<<endl;
    edge<<"       num_send_nodes 1"<<endl;
    edge<<"       send_nodes ( "<<from<<" )"<<endl;
    edge<<"       num_recv_nodes 2"<<endl;
    edge<<"       recv_nodes ( "<<to<<" )"<<endl;
    edge<<"            <SEND_BEGIN>"<<endl;
    edge<<"                <CHUNK_BEGIN>"<<endl;
    edge<<"                   name \"row\""<<endl;
    edge<<"                   type GRAPH_LONG"<<endl;
    edge<<"                   left_offset \"0\""<<endl;
    edge<<"                   right_offset \""<<5*sizeof(long)<<"+F_LEN\""<<endl;
    edge<<"                <CHUNK_END>"<<endl;
    edge<<"            <SEND_END>"<<endl;
    edge<<"            <RECIEVE_BEGIN>"<<endl;
    edge<<"                <CHUNK_BEGIN>"<<endl;
    edge<<"                   name \"row\""<<endl;
    edge<<"                   type GRAPH_LONG"<<endl;
    edge<<"                   left_offset \"0\""<<endl;
    edge<<"                   right_offset \""<<5*sizeof(long)<<"\""<<endl;
    edge<<"                <CHUNK_END>"<<endl;
    edge<<"            <RECIEVE_END>"<<endl;
    edge<<"     <EDGE_END>"<<endl<<endl;
}
//не забыть возврат из функции.
void printBeginNode()
{
    node<<"<NODE_BEGIN>"<<endl;
    node<<"    number "<<0<<endl;
    node<<"    type 0"<<endl;
    node<<"    weight 100"<<endl;
    node<<"    layer 2"<<endl;
    node<<"    num_input_edges 0"<<endl;
    node<<"    edges (  )"<<endl;
    node<<"    num_output_edges 1"<<endl;
    node<<"    edges ( 1 )"<<endl;
    node<<"    head \"\""<<endl;
    node<<"    body \"BigNode\""<<endl;
    node<<"    tail \"\""<<endl;
    node<<"<NODE_END>"<<endl<<endl;
}

int main()
{
	node.open("node.grf");
	edge.open("edge.grf");

	number_of_nodes=9;
    printBeginNode();
    int j=2;
    for (int i=1; i<number_of_nodes+1; i++)
    {
        int from = (int)(i/2) - (i%2);
    	int to = i*2;
    	if (to+1 <= number_of_nodes)
    	{ 
    		printedge(j,i,to);
    		j++;
    		printedge(j,i,to+1);
        	j++;
        }else{
        	if (to == number_of_nodes)
        	{
        		printedge(j,i,to);
        		j++;
        	}else{to=0;}
        }
        printnode(i,from,to);
    }
    return 0;
}
