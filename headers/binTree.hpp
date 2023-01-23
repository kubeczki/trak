#pragma once
#include "triangle.hpp"
#include <vector>
#include "pack.hpp"

class BinTree
{
private:
    std::vector<std::vector<Triangle>> grid;
    int dim;
    Range range;
public:
    BinTree(int nTriangles, Range r)
    {
        std::vector<Triangle> tmp;
        dim = int(std::cbrt(double(nTriangles)) * 3);
        //std::cout << dim << std::endl;
        int tmp2 = int(pow(dim, 3));
        for (int i = 0; i < tmp2; i++)
        {
            grid.push_back(tmp);
        }
        range = r;
    }
    void BinTree::append(int x, int y, int z, Triangle tri)
    {
        grid[z*dim*dim+y*dim+x].push_back(tri);
    }
    
    void BinTree::append(int k, Triangle tri)
    {
        grid[k].push_back(tri);
    }
    
    std::vector<Triangle> BinTree::get(int x, int y, int z) const
    {
        return grid[z * dim * dim + y * dim + x];
    }

    void BinTree::putInCubes(Pack p)
    {
        float tmp;
        
        int x_min = int((p.range.x_min - range.x_min) / (range.x_max - range.x_min) * dim);
        int y_min = int((p.range.y_min - range.y_min) / (range.y_max - range.y_min) * dim);
        int z_min = int((p.range.z_min - range.z_min) / (range.z_max - range.z_min) * dim);
        
        int x_max, y_max, z_max;
        
        tmp = (p.range.x_max - range.x_min) / (range.x_max - range.x_min) * dim;
        x_max = int(tmp);
        
        if (floor(tmp) == tmp && x_max > x_min)
        {
            x_max--;
        }
        
        tmp = (p.range.y_min - range.y_min) / (range.y_max - range.y_min) * dim;
        y_max = int(tmp);

        if (floor(tmp) == tmp && y_max > y_min)
        {
            y_max--;
        }

        tmp = (p.range.z_min - range.z_min) / (range.z_max - range.z_min) * dim;
        z_max = int(tmp);

        if (floor(tmp) == tmp && z_max > z_min)
        {
            z_max--;
        }

        for (int i = x_min; i <= x_max; i++)
        {
            for (int j = y_min; j <= y_max; j++)
            {
                for (int k = z_min; k <= z_max; k++)
                {
                    append(i, j, k, p.triangle);
                }
            }
        }
    }
    
    void BinTree::fill(std::vector<Pack> list)
    {
        for (int i=0; i<list.size();i++)
        {
            putInCubes(list[i]);
        }
    }

    int BinTree::size()
    {
        return int(grid.size());
    }

    int BinTree::getDim()
    {
        return dim;
    }
    
    std::vector<int> BinTree::intersectionsWithBoxes(Vector3f start, Vector3f dir)
    {
        std::vector<int> boxesThatIntersect;
        int x, y, z;
        float tx, ty, tz;
        int tmp, tm2;
        if (dir.x)
        {
            if (dir.y)
            {
                if (dir.z)
                {
                    x = int((start.x - range.x_min) / (range.x_max - range.x_min)) * dim;
                    y = int((start.y - range.y_min) / (range.y_max - range.y_min)) * dim;
                    z = int((start.z - range.z_min) / (range.z_max - range.z_min)) * dim;
                    if (dir.x > 0)
                    {
                        if (start.x > range.x_max)
                        {
                            return boxesThatIntersect;
                        }
                        if (dir.y > 0)
                        {
                            if (start.y > range.y_max)
                            {
                                return boxesThatIntersect;
                            }
                            if (dir.z > 0)
                            {
                                if (start.z > range.z_max)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x < 0) x = -1;
                                if (y < 0) y = -1;
                                if (z < 0) z = -1;
                                x++;
                                y++;
                                z++;
                                while (x<dim && y<dim && z<dim)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * y + tmp);
                                            y++;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x);
                                            x++;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (start.z < range.z_min)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x < 0) x = -1;
                                if (y < 0) y = -1;
                                if (z > dim) z = dim + 1;
                                x++;
                                y++;
                                z--;
                                while (x < dim && y < dim && z > 0)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * y + tmp);
                                            y++;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x);
                                            x++;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (start.y < range.y_min)
                            {
                                return boxesThatIntersect;
                            }
                            if (dir.z > 0)
                            {
                                if (start.z > range.z_max)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x < 0) x = -1;
                                if (y > dim) y = dim + 1;
                                if (z < 0) z = -1;
                                x++;
                                y--;
                                z++;
                                while (x < dim && y > 0 && z < dim)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * (y - 1) + tmp);
                                            y--;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x);
                                            x++;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (start.z < range.z_min)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x < 0) x = -1;
                                if (y > dim) y = dim + 1;
                                if (z > dim) z = dim + 1;
                                x++;
                                y--;
                                z--;
                                while (x < dim && y > 0 && z > 0)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * (y - 1)  + tmp);
                                            y--;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x);
                                            x++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if (start.x < range.x_min)
                        {
                            return boxesThatIntersect;
                        }
                        if (dir.y > 0)
                        {
                            if (start.y > range.y_max)
                            {
                                return boxesThatIntersect;
                            }
                            if (dir.z > 0)
                            {
                                if (start.z > range.z_max)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x > dim) x = dim + 1;
                                if (y < 0) y = -1;
                                if (z < 0) z = -1;
                                x--;
                                y++;
                                z++;
                                while (x > 0 && y < dim && z < dim)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * y + tmp);
                                            y++;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x - 1);
                                            x--;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (start.z < range.z_min)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x > dim) x = dim + 1;
                                if (y < 0) y = -1;
                                if (z > dim) z = dim + 1;
                                x--;
                                y++;
                                z--;
                                while (x > 0 && y < dim && z > 0)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * y + tmp);
                                            y++;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x - 1);
                                            x--;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (start.y < range.y_min)
                            {
                                return boxesThatIntersect;
                            }
                            if (dir.z > 0)
                            {
                                if (start.z > range.z_max)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x > dim) x = dim + 1;
                                if (y > dim) y = dim + 1;
                                if (z < 0) z = -1;
                                x--;
                                y--;
                                z++;
                                while (x > 0 && y > 0 && z < dim)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * (y - 1) + tmp);
                                            y--;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tm2 + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x - 1);
                                            x--;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (start.z < range.z_min)
                                {
                                    return boxesThatIntersect;
                                }
                                if (x > dim) x = dim + 1;
                                if (y > dim) y = dim + 1;
                                if (z > dim) z = dim + 1;
                                x--;
                                y--;
                                z--;
                                while (x > 0 && y > 0 && z > 0)
                                {
                                    tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                    ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                    tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                    if (tx >= ty)
                                    {
                                        if (ty >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            tm2 = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tm2 + dim * (y - 1) + tmp);
                                            y--;
                                        }
                                    }
                                    else
                                    {
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            tm2 = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tm2 + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            tm2 = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * tm2 + x - 1);
                                            x--;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    x = int((start.x - range.x_min) / (range.x_max - range.x_min)) * dim;
                    y = int((start.y - range.y_min) / (range.y_max - range.y_min)) * dim;
                    z = int((start.z - range.z_min) / (range.z_max - range.z_min)) * dim;
                    if (x >= 0 && y >= 0 && z >= 0 && x < dim && y < dim && z < dim)
                    {
                        boxesThatIntersect.push_back(dim* dim* z + dim * y + x);
                    }
                    
                    if (dir.x > 0)
                    {
                        if (start.x > range.x_max)
                        {
                            return boxesThatIntersect;
                        }
                        else
                        {
                            if (dir.y > 0)
                            {
                                if (start.y > range.y_max)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x < 0) x = -1;
                                    if (y < 0) y = -1;
                                    x++;
                                    y++;
                                    while (x<dim+1 && y<dim+1)
                                    {
                                        tx = (float(x) / dim * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        ty = (float(y) / dim * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tx >= ty)
                                        {
                                            if (y == dim) return boxesThatIntersect;
                                            tmp = int((start.x + ty * dir.x - range.x_min) / (range.x_max - range.x_min) * dim);
                                            boxesThatIntersect.push_back(dim*dim*z+dim*y+tmp);
                                            y++;
                                        }
                                        else
                                        {
                                            if (x == dim) return boxesThatIntersect;
                                            tmp = int((start.y + tx * dir.y - range.y_min) / (range.y_max - range.y_min)* dim);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tmp + x);
                                            x++;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                            else
                            {
                                if (start.y < range.y_min)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x < 0) x = -1;
                                    if (y > dim) y = dim+1;
                                    x++;
                                    y--;
                                    while (x < dim + 1 && y > 0)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tx >= ty)
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * (y-1) + tmp);
                                            y--;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tmp + x);
                                            x++;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (start.x < range.x_min)
                        {
                            return boxesThatIntersect;
                        }
                        else
                        {
                            if (dir.y > 0)
                            {
                                if (start.y > range.y_max)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x > dim) x = dim + 1;
                                    if (y < 0) y = -1;
                                    x--;
                                    y++;
                                    while (x > 0 && y < dim)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tx >= ty)
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * y + tmp);
                                            y++;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tmp + x - 1);
                                            x--;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                            else
                            {
                                if (start.y < range.y_min)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x > dim) x = dim + 1;
                                    if (y > dim) y = dim + 1;
                                    x--;
                                    y--;
                                    while (x > 0 && y > 0)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tx >= ty)
                                        {
                                            tmp = int(start.x + ty * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * (y - 1) + tmp);
                                            y--;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tx * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tmp + x - 1);
                                            x--;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (dir.z)
                {
                    x = int((start.x - range.x_min) / (range.x_max - range.x_min)) * dim;
                    y = int((start.y - range.y_min) / (range.y_max - range.y_min)) * dim;
                    z = int((start.z - range.z_min) / (range.z_max - range.z_min)) * dim;

                    if (dir.x > 0)
                    {
                        if (start.x > range.x_max)
                        {
                            return boxesThatIntersect;
                        }
                        else
                        {
                            if (dir.z > 0)
                            {
                                if (start.z > range.z_max)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x < 0) x = -1;
                                    if (z < 0) z = -1;
                                    x++;
                                    z++;
                                    while (x < dim && z < dim)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * y + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * y + x);
                                            x++;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                            else
                            {
                                if (start.z < range.z_min)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x < 0) x = -1;
                                    if (z > dim) z = dim + 1;
                                    x++;
                                    z--;
                                    while (x < dim && z > 0)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * y + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * y + x);
                                            x++;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (start.x < range.x_min)
                        {
                            return boxesThatIntersect;
                        }
                        else
                        {
                            if (dir.z > 0)
                            {
                                if (start.z > range.z_max)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x > dim) x = dim + 1;
                                    if (z < 0) z = -1;
                                    x--;
                                    z++;
                                    while (x > 0 && z < dim)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * y + tmp);
                                            z++;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * y + x - 1);
                                            x--;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                            else
                            {
                                if (start.z < range.z_min)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (x > dim) x = dim + 1;
                                    if (z > dim) z = dim + 1;
                                    x--;
                                    z--;
                                    while (x > 0 && z > 0)
                                    {
                                        tx = (float(x / dim) * (range.x_max - range.x_min) + range.x_min - start.x) / dir.x;
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        if (tx >= tz)
                                        {
                                            tmp = int(start.x + tz * dir.x);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * y + tmp);
                                            z--;
                                        }
                                        else
                                        {
                                            tmp = int(start.z + tx * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * y + x - 1);
                                            x--;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (start.z > range.z_min && start.z < range.z_max && start.y > range.y_min && start.y < range.y_max)
                    {
                        x = int((start.x - range.x_min) / (range.x_max - range.x_min)) * dim;
                        y = int((start.y - range.y_min) / (range.y_max - range.y_min)) * dim;
                        z = int((start.z - range.z_min) / (range.z_max - range.z_min)) * dim;
                        //std::cout << x << " " << y << " " << z << std::endl;
                        if (dir.x > 0)
                        {
                            //std::cout << x <<" "<<dim<< std::endl;
                            for (int i = x; i < dim; i++)
                            {
                                //std::cout << i << std::endl;
                                boxesThatIntersect.push_back(dim * dim * z + dim * y + i);
                            }
                            return boxesThatIntersect;
                        }
                        else
                        {
                            for (int i = 0; i <= x; i++)
                            {
                                boxesThatIntersect.push_back(dim * dim * z + dim * y + i);
                            }
                            return boxesThatIntersect;
                        }

                    }
                    else
                    {
                        return boxesThatIntersect;
                    }
                }
            }
        }
        else
        {
            if (dir.y)
            {
                if (dir.z)
                {
                    x = int((start.x - range.x_min) / (range.x_max - range.x_min)) * dim;
                    y = int((start.y - range.y_min) / (range.y_max - range.y_min)) * dim;
                    z = int((start.z - range.z_min) / (range.z_max - range.z_min)) * dim;

                    if (dir.z > 0)
                    {
                        if (start.z > range.z_max)
                        {
                            return boxesThatIntersect;
                        }
                        else
                        {
                            if (dir.y > 0)
                            {
                                if (start.y > range.y_max)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (z < 0) z = -1;
                                    if (y < 0) y = -1;
                                    z++;
                                    y++;
                                    while (z < dim && y < dim)
                                    {
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tz >= ty)
                                        {
                                            tmp = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * y + x);
                                            y++;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tmp + x);
                                            z++;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                            else
                            {
                                if (start.y < range.y_min)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (z < 0) z = -1;
                                    if (y > dim) y = dim + 1;
                                    z++;
                                    y--;
                                    while (z < dim && y > 0)
                                    {
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tz >= ty)
                                        {
                                            tmp = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * (y - 1) + x);
                                            y--;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * z + dim * tmp + x);
                                            z++;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (start.z < range.z_min)
                        {
                            return boxesThatIntersect;
                        }
                        else
                        {
                            if (dir.y > 0)
                            {
                                if (start.y > range.y_max)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (z > dim) z = dim + 1;
                                    if (y < 0) y = -1;
                                    z--;
                                    y++;
                                    while (z > 0 && y < dim)
                                    {
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tz >= ty)
                                        {
                                            tmp = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * y + x);
                                            y++;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tmp + x);
                                            z--;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                            else
                            {
                                if (start.y < range.y_min)
                                {
                                    return boxesThatIntersect;
                                }
                                else
                                {
                                    if (z > dim) z = dim + 1;
                                    if (y > dim) y = dim + 1;
                                    z--;
                                    y--;
                                    while (z > 0 && y > 0)
                                    {
                                        tz = (float(z / dim) * (range.z_max - range.z_min) + range.z_min - start.z) / dir.z;
                                        ty = (float(y / dim) * (range.y_max - range.y_min) + range.y_min - start.y) / dir.y;
                                        if (tz >= ty)
                                        {
                                            tmp = int(start.z + ty * dir.z);
                                            boxesThatIntersect.push_back(dim * dim * tmp + dim * (y - 1) + x);
                                            y--;
                                        }
                                        else
                                        {
                                            tmp = int(start.y + tz * dir.y);
                                            boxesThatIntersect.push_back(dim * dim * (z - 1) + dim * tmp + x);
                                            z--;
                                        }
                                    }
                                    return boxesThatIntersect;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (start.x > range.x_min && start.x < range.x_max && start.z > range.z_min && start.z < range.z_max)
                    {
                        x = int((start.x - range.x_min) / (range.x_max - range.x_min)) * dim;
                        y = int((start.y - range.y_min) / (range.y_max - range.y_min)) * dim;
                        z = int((start.z - range.z_min) / (range.z_max - range.z_min)) * dim;

                        if (dir.y > 0)
                        {
                            for (int i = y; i < dim; i++)
                            {
                                boxesThatIntersect.push_back(dim * dim * z + dim * i + x);
                            }
                            return boxesThatIntersect;
                        }
                        else
                        {
                            for (int i = 0; i <= y; i++)
                            {
                                boxesThatIntersect.push_back(dim * dim * z + dim * i + x);
                            }
                            return boxesThatIntersect;
                        }

                    }
                    else
                    {
                        return boxesThatIntersect;
                    }
                }
            }
            else
            {
                if (dir.z)
                {
                    if (start.x > range.x_min && start.x < range.x_max && start.y > range.y_min && start.y < range.y_max)
                    {
                        x = int(start.x * dim / (range.x_max - range.x_min));
                        y = int(start.y * dim / (range.y_max - range.y_min));
                        z = int(start.z * dim / (range.z_max - range.z_min));

                        if (dir.z > 0)
                        {
                            for (int i = z; i < dim; i++)
                            {
                                boxesThatIntersect.push_back(dim * dim * i + dim * y + x);
                            }
                            return boxesThatIntersect;
                        }
                        else
                        {
                            for (int i = 0; i <= z; i++)
                            {
                                boxesThatIntersect.push_back(dim * dim * i + dim * y + x);
                            }
                            return boxesThatIntersect;
                        }

                    }
                    else
                    {
                        return boxesThatIntersect;
                    }
                }
            }
        }
    }





};
