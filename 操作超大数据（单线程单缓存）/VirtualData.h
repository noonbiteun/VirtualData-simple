#if !defined(VIRTUALDATA_H_INCLUDE)
#define VIRTUALDATA_H_INCLUDE

#include <iostream>
#include <stdio.h>

template <typename T>
class VirtualData
{
private:
    T **buffer;
    int bufferID;
    int bs_z, bs_x, bs_y;       //buffer size
    int data_z, data_x, data_y; //data size
    int xcut, ycut, zcut;

    void SavaBuffer();
    void LoadBuffer(int);
    void UpdateBufferSize(int);
    bool UpdateBuffer(int);                   //更新Buffer数据
    int Coord2BufferID(int, int, int);           //坐标转换成ID
    void Absolute2Relative(int &, int &, int &); //绝对坐标转相对坐标

public:
    VirtualData(int, int, int, int, int, int);
    ~VirtualData();

    T& GetValue(int, int, int);
};

template <class T>
VirtualData<T>::VirtualData(int dx, int dy, int dz, int xc, int yc, int zc)
{
    data_x = dx;
    data_y = dy;
    data_z = dz;
    xcut = xc;
    ycut = yc;
    zcut = zc;
    bufferID = -1;
    bs_x = data_x / xcut;
    bs_y = data_y / ycut;
    bs_z = data_z / zcut;

    buffer = new T*[bs_z];
    for (int i = 0; i < bs_z; i++)
    {
        buffer[i] = new T[bs_x*bs_y];
    }
}

template <class T>
VirtualData<T>::~VirtualData()
{
    SavaBuffer();
    if (buffer)
    {
        int xysize = sizeof(buffer[0]);
        for (int fz = 0; fz < sizeof(buffer) / xysize; fz++)
        {
            delete[] buffer[fz];
        }
        delete[] buffer; //删除buffer区
    }
}

template <class T>
int VirtualData<T>::Coord2BufferID(int x, int y, int z)
{
    int xid, yid, zid;
    //坐标置换成子图像的ID
    xid = x * xcut / data_x;
    yid = y * ycut / data_y;
    zid = z * zcut / data_z;
    return xid + yid * xcut + zid * xcut * ycut;
}

template <class T>
void VirtualData<T>::Absolute2Relative(int &x, int &y, int &z)
{
    int xid, yid, zid;
    //坐标置换成子图像的ID
    xid = x * xcut / data_x;
    yid = y * ycut / data_y;
    zid = z * zcut / data_z;

    x -= data_x / xcut * xid;
    y -= data_y / ycut * yid;
    z -= data_z / zcut * zid;
}

template <class T>
void VirtualData<T>::UpdateBufferSize(int id)
{
    //将ID置换成三维方向的分量ID
    int xid, yid, zid;
    xid = id % xcut;
    yid = (id % (xcut * ycut)) / xcut;
    zid = id / (xcut * ycut);

    //计算大小
    int newbsx, newbsy, newbsz;
    if (xid != (xcut - 1))
    {
        newbsx = data_x / xcut;
    }
    else
    {
        newbsx = data_x - data_x / xcut * (xcut - 1);
    }
    if (yid != (ycut - 1))
    {
        newbsy = data_y / ycut;
    }
    else
    {
        newbsy = data_y - data_y / ycut * (ycut - 1);
    }
    if (zid != (zcut - 1))
    {
        newbsz = data_z / zcut;
    }
    else
    {
        newbsz = data_z - data_z / zcut * (zcut - 1);
    }
    //判断大小是否发生变化
    if (bs_x != newbsx || bs_y != newbsy || bs_z != newbsz)
    {
        //删除旧的buffer区
        for (int i = 0; i < bs_z; i++)
        {
            delete[] buffer[i];
        }
        delete[] buffer;
        //更新 buffer size
        bs_x = newbsx;
        bs_y = newbsy;
        bs_z = newbsz;
        //生成新的buffer区
        buffer = new T*[bs_z];
        for (int j = 0; j < bs_z; j++)
        {
            buffer[j] = new T[bs_x*bs_y];
        }
    }
}

template <class T>
void VirtualData<T>::SavaBuffer()
{
    if (bufferID < 0)
    {
        return;
    }
    char filename[20];
    sprintf(filename, "./tmpdata/%04d.dat", bufferID);
    FILE *tmpdat = fopen(filename, "w+b");
    if (!tmpdat)
    {
        printf("SavaBuffer : open file failed~~~\n");
        return;
    }
    for (int i = 0; i < bs_z; i++)
    {
        fwrite(buffer[i], sizeof(T), bs_x * bs_y, tmpdat);
    }
    fclose(tmpdat);
}

template <class T>
void VirtualData<T>::LoadBuffer(int id)
{
    char filename[20];
    sprintf(filename, "./tmpdata/%04d.dat", id);
    FILE *tmpdat = fopen(filename, "r+b");
    if (!tmpdat)
    {
        printf("LoadBuffer : open file failed~~~\n");
        return;
    }
    printf("%d %d %d\n", bs_x, bs_y, bs_z);
    for (int i = 0; i < bs_z; i++)
    {
        fread(buffer[i], sizeof(T), bs_x * bs_y, tmpdat);
    }
    
    fclose(tmpdat);
}

template <class T>
bool VirtualData<T>::UpdateBuffer(int id)
{
    UpdateBufferSize(id);
    SavaBuffer();
    LoadBuffer(id);
    bufferID = id;
}

template <class T>
T& VirtualData<T>::GetValue(int x, int y, int z)
{
    int atID = Coord2BufferID(x, y, z);
    // printf("BufferID : %d *** ", atID);
    if (atID != bufferID)
    {
        //不在当前加载的Buffer中
        UpdateBuffer(atID);
    }
    //坐标转换
    // printf("coord : (%d,%d,%d) ---> ", x, y, z);
    Absolute2Relative(x, y, z);
    // printf("(%d,%d,%d)   ", x, y, z);
    // printf("value(%d)\n", buffer[z][x + y * bs_x]);
    return buffer[z][x + y * bs_x];
}

#endif // VIRTUALDATA_H_INCLUDE
