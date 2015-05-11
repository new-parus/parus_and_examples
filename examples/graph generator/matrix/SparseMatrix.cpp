#include "SparseMatrix.h"

using namespace std;


void SparseMatix::CopyPartOfCol(long bin, long end, long *C, double *elem) const
{
    for (long i=bin;i<end;i++)
    {
        C[i-bin]=col_mass[i];
        elem[i-bin]=element_mass[i];
    }
}

void SparseMatix::CopyPartOfRow(long bin, long end, long *R, double *elem) const
{
    for (long i=bin;i<end;i++)
    {
        R[i-bin]=row_mass[i];
        elem[i-bin]=element_mass[i];
    }
}

long SparseMatix::GetNumbOfRows() const
{
    long i=1, num=1;
    double remember;
    remember = row_mass[0];
    for (i=1;i<size_of_mass;i++)
        if (remember != row_mass[i])
        {
            remember = row_mass[i];
            num++;
        }
    return num;
}

void SparseMatix::CreateMatrix(const char *filename)
{
    FILE *fb=NULL;
    fb=fopen (filename,"r");
    max_row=0;
    max_col=0;
    size_of_mass=0;
    delete[] element_mass;
    delete[] row_mass;
    delete[] col_mass;
    fscanf(fb,"%i%i%i\n",&max_row,&max_col,&size_of_mass);
    element_mass=new double[size_of_mass];
    col_mass=new long[size_of_mass];
    row_mass=new long[size_of_mass];
    for (long i=0; i<size_of_mass;i++)
    {
        float tmp;
        fscanf(fb,"%i%i%f\n",&row_mass[i], &col_mass[i], &tmp);
        element_mass[i]=tmp;
    }
    if(fclose(fb)) printf("Ошибка при закрытии файла.\n");
}

void SparseMatix::AddElement(long r,long c,double el)
{
    size_of_mass++;
    long *row= new long[size_of_mass];
    for (long i=0;i<size_of_mass-1;i++)
        row[i]=row_mass[i];
    row[size_of_mass-1]=r;
    delete[] row_mass;

    long *col= new long[size_of_mass];
    for (long i=0;i<size_of_mass-1;i++)
        col[i]=col_mass[i];
    col[size_of_mass-1]=c;
    delete[] col_mass;

    double *elem= new double[size_of_mass];
    for (long i=0;i<size_of_mass-1;i++)
        elem[i]=element_mass[i];
    elem[size_of_mass-1]=el;
    delete[] element_mass;

    row_mass=row;
    col_mass=col;
    element_mass=elem;
}

void SparseMatix::PrintMatrix(const char *filename)
{
    FILE *fb=NULL;
    fb=fopen (filename,"w");
    fprintf(fb,"%i %i %i\nMatrix in format rce\n",max_row,max_col,size_of_mass);
    for (long i=0; i<size_of_mass;i++)
    {
        float tmp=(float)element_mass[i];
        fprintf(fb,"%i %i %f\n",row_mass[i], col_mass[i], tmp);
    }
    if(fclose(fb)) printf("Ошибка при закрытии файла.\n");
}

void SparseMatix::PrintMatrix2()
{
    cout<< max_row << " "<<max_col<<" "<<size_of_mass<<endl<<"Matrix in format rce:"<<endl;
    for (long i=0; i<size_of_mass;i++)
    {
        cout << (int)row_mass[i]<< " ";
        cout << (int)col_mass[i]<< " ";
        cout << element_mass[i]<<endl;
    }
    cout << endl;
}

long* SparseMatix::LocalSort(long* sorting, const long* id)
{
    long* s = new long[size_of_mass];
    for (long i=0;i<size_of_mass;i++)
    {
        s[i]=sorting[id[i]];
    }
    delete[] sorting;
    return s;
}

double* SparseMatix::LocalSortD(double* sorting, const long* id)
{
    double *s = new double[size_of_mass];
    for (long i=0;i<size_of_mass;i++)
    {
        s[i]=sorting[id[i]];
    }
    delete[] sorting;
    return s;
}
void quickSort(long begin, long end, long* mass, long* mass2, long* id)
{
    long x = (int)mass[begin];
    long y = (int)mass2[begin];
    long i = begin;
    long j = end;
    while(i <= j)
    {
        while( ((int)mass[i] < x) || (((int)mass[i]==x)&&((int)mass2[i]<y)) ) i++;
        while( ((int)mass[j] > x) || (((int)mass[j]==x)&&((int)mass2[j]>y)) ) j--;
        if(i <= j)
        {
            long temp=mass[i];
            mass[i]=mass[j];
            mass[j]=temp;
            temp=mass2[i];
            mass2[i]=mass2[j];
            mass2[j]=temp;
            temp=id[i];
            id[i]=id[j];
            id[j]=temp;
            i++;
            j--;
        }
    }
    if (i<end)
        quickSort(i, end, mass, mass2, id);

    if (begin<j)
        quickSort(begin, j,  mass, mass2, id);
}


void SparseMatix::SortForRow()
{
    long* id= new long[size_of_mass];
    for (long i=0; i<size_of_mass;i++)
        id[i]=i;
    quickSort(0,size_of_mass-1,row_mass,col_mass,id);
    //for (long i=0; i<size_of_mass;i++)
    //    cout<<row_mass[i];
    //col_mass=LocalSort(col_mass, id);
    element_mass=LocalSortD(element_mass, id);
    delete[] id;
}

void SparseMatix::SortForCol()
{
    long* id= new long[size_of_mass];
    for (long i=0; i<size_of_mass;i++)
        id[i]=i;
    quickSort(0,size_of_mass-1,col_mass,row_mass,id);
    //for (long i=0; i<size_of_mass;i++)
    //   cout<<(int)col_mass[i];
    //row_mass=LocalSort(row_mass, id);
    element_mass=LocalSortD(element_mass, id);
    delete[] id;
}

void SparseMatix::GetRow(const long number,long &begin_row,long &end_row) const
{
    long i;
    begin_row=0;
    end_row=0;
    for (i=0;i<size_of_mass;i++)
        if ((int)row_mass[i]==(int)number) break;
    if (i==size_of_mass) return;
    begin_row=i;
    while  ((int)number==(int)row_mass[i])
        i++;
    end_row=i;
}
void SparseMatix::GetCol(const long number,long &c_begin,long &c_end) const
{
    long i;
    c_begin=0;
    c_end=0;
    for (i=0;i<size_of_mass;i++)
        if ((int)col_mass[i]==(int)number) break;
    if (i==size_of_mass) return;
    c_begin=i;
    while  ((int)number==(int)col_mass[i])
        i++;
    c_end=i;
}

