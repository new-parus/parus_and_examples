#include <iostream>
#include <list>
#include <algorithm>
#include <vector>
#include <stdio.h>

using namespace std;

bool comp_row(long a, long b);
bool comp_col(long a, long b);

class SparseMatix
{
    double* element_mass;
    long* row_mass;
    long* col_mass;
    long max_col;
    long max_row;
    long size_of_mass;
    long* LocalSort(long* sort,const long* id);
    double* LocalSortD(double* sort,const long* id);
public:
    SparseMatix()
    {
        size_of_mass=0;
        element_mass=new double[0];
        row_mass=new long[0];
        col_mass=new long[0];
    }
    ~SparseMatix()
    {
        delete[] element_mass;
        delete[] row_mass;
        delete[] col_mass;
    }
    void CreateMatrix(const char *filename);
    void AddElement(long r,long c,double el);
    void PrintMatrix2();
    void PrintMatrix(const char *filename);
    void SortForRow();
    void SortForCol();
    void SetMaxCol(const long m){max_col=m;}
    void SetMaxRow(const long m){max_row=m;}
    void SetSizeOfMass(const long m){size_of_mass=m;}
    long GetMaxCol()const {return max_col;}
    long GetMaxRow()const {return max_row;}
    long GetSizeOfMass()const {return size_of_mass;}
    long* GetRows()const {return row_mass;}
    long* GetCols()const {return col_mass;}
    double* GetElemrnts(){return element_mass;}
    long Row_mass(long k) const {return row_mass[k];}
    long Col_mass(long k) const {return col_mass[k];}
    double Element_mass(long k) const {return element_mass[k];}
    void GetRow(const long number,long &begin_row,long &end_row) const;
    void GetCol(const long number,long &c_begin,long &c_end) const;
    long GetNumbOfRows() const;
    void CopyPartOfCol(long bin, long end, long *C, double *elem) const;
    void CopyPartOfRow(long bin, long end, long *R, double *elem) const;
};
