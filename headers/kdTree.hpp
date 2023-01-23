#pragma once
#include "triangle.hpp"
#include "pack.hpp"
#include <algorithm>

class KdNode
{
public:
    std::vector<Triangle> list;
    Range range;

    std::vector<KdNode> children;

    KdNode(std::vector<Pack> l, int dim, Range r, Vector3f biggest)
    {
        //std::cout << l.size() << std::endl;
        std::vector<Pack> list_x = l;
        std::vector<Pack> list_y = l;
        std::vector<Pack> list_z = l;
        
        switch (dim)
        {
            case 0:
            {
                std::sort(list_y.begin(), list_y.end(), comp_y);
                std::sort(list_z.begin(), list_z.end(), comp_z);
                break;
            }
            case 1:
            {
                std::sort(list_x.begin(), list_x.end(), comp_x);
                std::sort(list_z.begin(), list_z.end(), comp_z);
                break;
            }
            case 2:
            {
                std::sort(list_y.begin(), list_y.end(), comp_y);
                std::sort(list_x.begin(), list_x.end(), comp_x);
            }
        }

        range = r;

        if (l.size() > 1) // hardcode
        {
            split(list_x, list_y, list_z, biggest);
        }

        int i;

        for (i = 0; i < list_x.size(); i++)
        {
            list.push_back(list_x[i].triangle);
        }

        Range tmp = r;

        range.x_min = list_x[0].range.x_min;
        range.y_min = list_y[0].range.y_min;
        range.z_min = list_z[0].range.z_min;

        range.x_max = list_x[list_x.size() - 1].range.x_max;
        range.y_max = list_y[list_y.size() - 1].range.y_max;
        range.z_max = list_z[list_z.size() - 1].range.z_max;

        float min;
        min = range.x_max - biggest.x;
        for (i = int(list_x.size() - 1); i >= 0; i--)
        {
            if (list_x[i].range.x_min < min)
            {
                break;
            }
        }
        i++;
        for (int j = i; j < list_x.size(); j++)
        {
            if (list_x[j].range.x_max > range.x_max)
            {
                range.x_max = list_x[j].range.x_max;
            }
        }

        min = range.y_max - biggest.y;
        for (i = int(list_y.size() - 1); i >= 0; i--)
        {
            if (list_y[i].range.y_min < min)
            {
                break;
            }
        }
        i++;
        for (int j = i; j < list_y.size(); j++)
        {
            if (list_y[j].range.y_max > range.y_max)
            {
                range.y_max = list_y[j].range.y_max;
            }
        }

        min = range.z_max - biggest.z;
        for (i = int(list_z.size() - 1); i >= 0; i--)
        {
            if (list_z[i].range.z_min < min)
            {
                break;
            }
        }
        i++;
        for (int j = i; j < list_z.size(); j++)
        {
            if (list_z[j].range.z_max > range.z_max)
            {
                range.z_max = list_z[j].range.z_max;
            }
        }

        if (range.x_min < tmp.x_min) range.x_min = tmp.x_min;
        if (range.y_min < tmp.y_min) range.y_min = tmp.y_min;
        if (range.z_min < tmp.z_min) range.z_min = tmp.z_min;
        if (range.x_max > tmp.x_max) range.x_max = tmp.x_max;
        if (range.y_max > tmp.y_max) range.y_max = tmp.y_max;
        if (range.z_max > tmp.z_max) range.z_max = tmp.z_max;
    }

    float KdNode::trySplit(int dim, int cutpoint, std::vector<Pack> l, Vector3f biggest)
    {
        int i;
        float min;
        float border;
        switch (dim)
        {
            case 0:
            {
                border = l[cutpoint].range.x_min;
                min = border - biggest.x;
                for (i = cutpoint - 1; i >= 0; i--)
                {
                    if (l[i].range.x_min < min)
                    {
                        break;
                    }
                }
                i++;
                for (int j = i; j < cutpoint; j++)
                {
                    if (l[j].range.x_max > border)
                    {
                        return 0;
                    }
                }
                return border;
            }
            case 1:
            {
                border = l[cutpoint].range.y_min;
                min = border - biggest.y;
                for (i = cutpoint - 1; i >= 0; i--)
                {
                    if (l[i].range.y_min < min)
                    {
                        break;
                    }
                }
                i++;
                for (int j = i; j < cutpoint; j++)
                {
                    if (l[j].range.y_max > border)
                    {
                        return 0;
                    }
                }
                return border;
            }
            case 2:
            {
                border = l[cutpoint].range.z_min;
                min = border - biggest.z;
                for (i = cutpoint - 1; i >= 0; i--)
                {
                    if (l[i].range.z_min < min)
                    {
                        break;
                    }
                }
                i++;
                for (int j = i; j < cutpoint; j++)
                {
                    if (l[j].range.z_max > border)
                    {
                        return 0;
                    }
                }
                return border;
            }
        }
        return 0;
    }

    void KdNode::split(std::vector<Pack> list_x, std::vector<Pack> list_y, std::vector<Pack> list_z, Vector3f biggest)
    {
        int mid = int(list_x.size()/2) - 1;

        int dim, midpoint = mid;
        float border = 0;

        bool flag = 1;

        int deviation = 0;

        while (deviation <= mid)
        {
            midpoint = mid - deviation + 1;
            border = trySplit(0, midpoint, list_x, biggest);
            if (border)
            {
                std::cout << midpoint << " " << border << std::endl;
                dim = 0;
                flag = 0;
                std::cout << "Split naturalny na x" << std::endl;
                break;
            }
            border = trySplit(1, midpoint, list_y, biggest);
            if (border)
            {
                dim = 1;
                flag = 0;
                std::cout << "Split naturalny na y" << std::endl;
                break;
            }
            border = trySplit(2, midpoint, list_z, biggest);
            if (border)
            {
                dim = 2;
                flag = 0;
                std::cout << "Split naturalny na z" << std::endl;
                break;
            }
            if (deviation < mid)
            {
                midpoint = mid + deviation + 1;
                border = trySplit(0, midpoint, list_x, biggest);
                if (border)
                {
                    dim = 0;
                    flag = 0;
                    std::cout << "Split naturalny dodatni na x" << std::endl;
                    break;
                }
                border = trySplit(1, midpoint, list_y, biggest);
                if (border)
                {
                    dim = 1;
                    flag = 0;
                    std::cout << "Split naturalny dodatni na y" << std::endl;
                    break;
                }
                border = trySplit(2, midpoint, list_z, biggest);
                if (border)
                {
                    dim = 2;
                    flag = 0;
                    std::cout << "Split naturalny dodatni na z" << std::endl;
                    break;
                }
            }            
            deviation++;
        }

        if (flag)
        {
            midpoint = mid + 1;
            if (range.x_max-range.x_min >= range.y_max - range.y_min)
            {
                if (range.x_max - range.x_min >= range.z_max - range.z_min)
                {
                    dim = 0;
                    border = list_x[midpoint].range.x_min;
                    std::cout << "Split wymuszony na x" << std::endl;
                }
                else
                {
                    dim = 2;
                    border = list_z[midpoint].range.z_min;
                    std::cout << "Split wymuszony na z" << std::endl;
                }
            }
            else
            {
                if (range.y_max - range.y_min >= range.z_max - range.z_min)
                {
                    dim = 1;
                    border = list_y[midpoint].range.y_min;
                    std::cout << "Split wymuszony na y" << std::endl;
                }
                else
                {
                    dim = 2;
                    border = list_z[midpoint].range.z_min;
                    std::cout << "Split wymuszony na z" << std::endl;
                }
            }
        }

        Range left = range, right = range;

        std::vector<Pack>::const_iterator first, middle, last;

        int i;
        float min;

        switch (dim)
        {
            case 0:
            {
                left.x_min = list_x[0].range.x_min;
                left.x_max = border;
                right.x_min = list_x[midpoint - 1].range.x_min;

                first = list_x.begin();
                middle = list_x.begin() + midpoint;
                last = list_x.end();
                break;
            }
            case 1:
            {
                left.y_min = list_y[0].range.y_min;
                left.y_max = border;
                right.y_min = list_y[midpoint - 1].range.y_min;

                first = list_y.begin();
                middle = list_y.begin() + midpoint;
                last = list_y.end();
                break;
            }
            case 2:
            {
                left.z_min = list_z[0].range.z_min;
                left.z_max = border;
                right.z_min = list_z[midpoint - 1].range.z_min;

                first = list_z.begin();
                middle = list_z.begin() + midpoint;
                last = list_z.end();
            }
        }
        
        std::vector<Pack> tmp(first, middle);
        std::vector<Pack> tm2(middle, last);
        std::vector<Pack> tm3;
        if (flag)
        {
            switch (dim)
            {
                case 0:
                {
                    min = border - biggest.x;
                    for (i = midpoint - 1; i >= 0; i--)
                    {
                        if (tmp[i].range.x_min < min)
                        {
                            break;
                        }
                    }
                    i++;
                    for (int j = i; j < tmp.size(); j++)
                    {
                        if (tmp[j].range.x_max > border)
                        {
                            tm3.push_back(tmp[j]);
                        }
                    }
                    break;
                }
                case 1:
                {
                    min = border - biggest.y;
                    for (i = midpoint - 1; i >= 0; i--)
                    {
                        if (tmp[i].range.y_min < min)
                        {
                            break;
                        }
                    }
                    i++;
                    for (int j = i; j < tmp.size(); j++)
                    {
                        if (tmp[j].range.y_max > border)
                        {
                            tm3.push_back(tmp[j]);
                        }
                    }
                    break;
                }
                case 2:
                {
                    min = border - biggest.z;
                    for (i = midpoint - 1; i >= 0; i--)
                    {
                        if (tmp[i].range.z_min < min)
                        {
                            break;
                        }
                    }
                    i++;
                    for (int j = i; j < tmp.size(); j++)
                    {
                        if (tmp[j].range.z_max > border)
                        {
                            tm3.push_back(tmp[j]);
                        }
                    }
                }
            }
            tm3.insert(tm3.end(), tm2.begin(), tm2.end());
        }

        if (tmp.size() < list_x.size() && tm2.size() < list_x.size() && tm3.size() < list_x.size())
        {
            KdNode child(tmp, dim, left, biggest);
            children.push_back(child);
            if (flag)
            {
                KdNode child2(tm3, dim, right, biggest);
                children.push_back(child2);
            }
            else
            {
                KdNode child2(tm2, dim, right, biggest);
                children.push_back(child2);
            }
        }
    }

    void KdNode::display()
    {
        std::cout << "Granice BB wezla: "<< std::endl;
        std::cout << "  x: " << range.x_min << " " << range.x_max << std::endl;
        std::cout << "  y: " << range.y_min << " " << range.y_max << std::endl;
        std::cout << "  z: " << range.z_min << " " << range.z_max << std::endl;
        std::cout << "Liczba trojkatow pod wezlem: " << list.size() << std::endl;
        for (int i = 0; i < list.size(); i++)
        {
            std::cout << i + 1 << ":\n";
            std::cout << list[i].v0.x << " " << list[i].v0.y << " " << list[i].v0.z << std::endl;
            std::cout << list[i].v1.x << " " << list[i].v1.y << " " << list[i].v1.z << std::endl;
            std::cout << list[i].v2.x << " " << list[i].v2.y << " " << list[i].v2.z << std::endl;
        }
        if (children.empty())
        {
            std::cout << "Jest to lisc\n";
        }
        else
        {
            std::cout << "Wezel ma " << children.size() << " dzieci\n";
        }
    }

    std::pair<float, Triangle> findIntersection(Vector3f start, Vector3f dir)
    {
        if (children.empty())
        {
            //
        }
        else
        {
            float withChild1 = -1, withChild2 = -1;
            Range child1 = children[0].range;
            Range child2 = children[1].range;
            float tx1, tx2, ty1, ty2, tz1, tz2;
            float tmp, tm2;

            if (dir.x)
            {
                if (dir.y)
                {
                    if (dir.z)
                    {
                        tx1 = (child1.x_min - start.x) / dir.x;
                        ty1 = (child1.y_min - start.y) / dir.y;
                        tz1 = (child1.z_min - start.z) / dir.z;
                        tx2 = (child1.x_max - start.x) / dir.x;
                        ty2 = (child1.y_max - start.y) / dir.y;
                        tz1 = (child1.z_min - start.z) / dir.z;

                        tmp = std::max(tx1, std::min(ty1, tz1));
                        tm2 = std::min(tx2, std::min(ty2, tz2));

                        if (tmp < tm2)
                        {
                            withChild1 = tmp;
                            if (withChild1 < 0) withChild1 = 0;
                        }

                        tx1 = (child2.x_min - start.x) / dir.x;
                        ty1 = (child2.y_min - start.y) / dir.y;
                        tz1 = (child2.z_min - start.z) / dir.z;
                        tx2 = (child2.x_max - start.x) / dir.x;
                        ty2 = (child2.y_max - start.y) / dir.y;
                        tz1 = (child2.z_min - start.z) / dir.z;

                        tmp = std::max(tx1, std::min(ty1, tz1));
                        tm2 = std::min(tx2, std::min(ty2, tz2));

                        if (tmp < tm2)
                        {
                            withChild2 = tmp;
                            if (withChild2 < 0) withChild2 = 0;
                        }
                    }
                    else
                    {
                        if (child1.z_min <= start.z && start.z <= child1.z_max)
                        {
                            tx1 = (child1.x_min - start.x) / dir.x;
                            ty1 = (child1.y_min - start.y) / dir.y;
                            tx2 = (child1.x_max - start.x) / dir.x;
                            ty2 = (child1.y_max - start.y) / dir.y;

                            tmp = std::max(tx1, ty1);
                            tm2 = std::min(tx2, ty2);
                            if (tmp < tm2)
                            {
                                withChild1 = tmp;
                                if (withChild1 < 0) withChild1 = 0;
                            }
                        }

                        if (child2.z_min <= start.z && start.z <= child2.z_max)
                        {
                            tx1 = (child2.x_min - start.x) / dir.x;
                            ty1 = (child2.y_min - start.y) / dir.y;
                            tx2 = (child2.x_max - start.x) / dir.x;
                            ty2 = (child2.y_max - start.y) / dir.y;

                            tmp = std::max(tx1, ty1);
                            tm2 = std::min(tx2, ty2);
                            if (tmp < tm2)
                            {
                                withChild2 = tmp;
                                if (withChild2 < 0) withChild2 = 0;
                            }
                        }
                    }
                }
                else
                {
                    if (dir.z)
                    {
                        if (child1.y_min <= start.y && start.y <= child1.y_max)
                        {
                            tz1 = (child1.z_min - start.z) / dir.z;
                            tx1 = (child1.x_min - start.x) / dir.x;
                            tz2 = (child1.z_max - start.z) / dir.z;
                            tx2 = (child1.x_max - start.x) / dir.x;

                            tmp = std::max(tz1, tx1);
                            tm2 = std::min(tz2, tx2);
                            if (tmp < tm2)
                            {
                                withChild1 = tmp;
                                if (withChild1 < 0) withChild1 = 0;
                            }
                        }

                        if (child2.y_min <= start.y && start.y <= child2.y_max)
                        {
                            tz1 = (child2.z_min - start.z) / dir.z;
                            tx1 = (child2.x_min - start.x) / dir.x;
                            tz2 = (child2.z_max - start.z) / dir.z;
                            tx2 = (child2.x_max - start.x) / dir.x;

                            tmp = std::max(tz1, tx1);
                            tm2 = std::min(tz2, tx2);
                            if (tmp < tm2)
                            {
                                withChild2 = tmp;
                                if (withChild2 < 0) withChild2 = 0;
                            }
                        }
                    }
                    else
                    {
                        if (start.z > child1.z_min && start.z < child1.z_max && start.y > child1.y_min && start.y < child1.y_max)
                        {
                            if (dir.x > 0)
                            {
                                if (child1.x_max > start.x)
                                {
                                    withChild1 = child1.x_min - start.x;
                                    if (withChild1 < 0) withChild1 = 0;
                                }
                            }
                            else
                            {
                                if (child1.x_min < start.x)
                                {
                                    withChild1 = start.x - child1.x_max;
                                    if (withChild1 < 0) withChild1 = 0;
                                }
                            }
                        }
                        if (start.z > child2.z_min && start.z < child2.z_max && start.y > child2.y_min && start.y < child2.y_max)
                        {
                            if (dir.x > 0)
                            {
                                if (child2.x_max > start.x)
                                {
                                    withChild2 = child2.x_min - start.x;
                                    if (withChild2 < 0) withChild2 = 0;
                                }
                            }
                            else
                            {
                                if (child2.x_min < start.x)
                                {
                                    withChild2 = start.x - child2.x_max;
                                    if (withChild2 < 0) withChild2 = 0;
                                }
                            }
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
                        if (child1.x_min <= start.x && start.x <= child1.x_max)
                        {
                            tz1 = (child1.z_min - start.z) / dir.z;
                            ty1 = (child1.y_min - start.y) / dir.y;
                            tz2 = (child1.z_max - start.z) / dir.z;
                            ty2 = (child1.y_max - start.y) / dir.y;

                            tmp = std::max(tz1, ty1);
                            tm2 = std::min(tz2, ty2);
                            if (tmp < tm2)
                            {
                                withChild1 = tmp;
                                if (withChild1 < 0) withChild1 = 0;
                            }
                        }
                        
                        if (child2.x_min <= start.x && start.x <= child2.x_max)
                        {
                            tz1 = (child2.z_min - start.z) / dir.z;
                            ty1 = (child2.y_min - start.y) / dir.y;
                            tz2 = (child2.z_max - start.z) / dir.z;
                            ty2 = (child2.y_max - start.y) / dir.y;

                            tmp = std::max(tz1, ty1);
                            tm2 = std::min(tz2, ty2);
                            if (tmp < tm2)
                            {
                                withChild2 = tmp;
                                if (withChild2 < 0) withChild2 = 0;
                            }
                        }
                    }
                    else
                    {
                        if (start.x > child1.x_min && start.x < child1.x_max && start.z > child1.z_min && start.z < child1.z_max)
                        {
                            if (dir.y > 0)
                            {
                                if (child1.y_max > start.y)
                                {
                                    withChild1 = child1.y_min - start.y;
                                    if (withChild1 < 0) withChild1 = 0;
                                }
                            }
                            else
                            {
                                if (child1.y_min < start.y)
                                {
                                    withChild1 = start.y - child1.y_max;
                                    if (withChild1 < 0) withChild1 = 0;
                                }
                            }
                        }
                        if (start.x > child2.x_min && start.x < child2.x_max && start.z > child2.z_min && start.z < child2.z_max)
                        {
                            if (dir.y > 0)
                            {
                                if (child2.y_max > start.y)
                                {
                                    withChild2 = child2.y_min - start.y;
                                    if (withChild2 < 0) withChild2 = 0;
                                }
                            }
                            else
                            {
                                if (child2.y_min < start.y)
                                {
                                    withChild2 = start.y - child2.y_max;
                                    if (withChild2 < 0) withChild2 = 0;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (dir.z)
                    {
                        if (start.x > child1.x_min && start.x < child1.x_max && start.y > child1.y_min && start.y < child1.y_max)
                        {
                            if (dir.z > 0)
                            {
                                if (child1.z_max > start.z)
                                {
                                    withChild1 = child1.z_min - start.z;
                                    if (withChild1 < 0) withChild1 = 0;
                                }
                            }
                            else
                            {
                                if (child1.z_min < start.z)
                                {
                                    withChild1 = start.z - child1.z_max;
                                    if (withChild1 < 0) withChild1 = 0;
                                }
                            }
                        }
                        if (start.x > child2.x_min && start.x < child2.x_max && start.y > child2.y_min && start.y < child2.y_max)
                        {
                            if (dir.z > 0)
                            {
                                if (child2.z_max > start.z)
                                {
                                    withChild2 = child2.z_min - start.z;
                                    if (withChild2 < 0) withChild2 = 0;
                                }
                            }
                            else
                            {
                                if (child2.z_min < start.z)
                                {
                                    withChild2 = start.z - child2.z_max;
                                    if (withChild2 < 0) withChild2 = 0;
                                }
                            }
                        }
                    }
                }
            }

            if (withChild1)
            {
                if (withChild2)
                {
                    if (withChild1 <= withChild2)
                    {
                        if (children[0].findIntersection(start, dir).first > 0)
                        {
                            return children[0].findIntersection(start, dir);
                        }
                        else
                        {
                            return children[1].findIntersection(start, dir);
                        }
                    }
                    else
                    {
                        if (children[1].findIntersection(start, dir).first > 0)
                        {
                            return children[1].findIntersection(start, dir);
                        }
                        else
                        {
                            return children[0].findIntersection(start, dir);
                        }
                    }
                }
                else
                {
                    return children[0].findIntersection(start, dir);
                }
            }
            else
            {
                if (withChild2)
                {
                    return children[1].findIntersection(start, dir);
                }
                else
                {
                    std::pair<float, Triangle> p (-1, list[0]);
                    return p;
                }
            }
        }
    }


};
