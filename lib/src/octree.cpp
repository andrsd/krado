// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// This is a re-write of OctTree from GMSH

#include "krado/octree.h"
#include "krado/log.h"

namespace krado {

OctTree::OctTree(int max_elements, const BoundingBox3D & bbox)
{
    double origin[3] = { bbox.min().x, bbox.min().y, bbox.min().z };
    double size[3] = { bbox.size()[0], bbox.size()[1], bbox.size()[3] };
    initialize_octant_buckets(origin, size, max_elements, &this->root, &this->info);
}

void
OctTree::free_buckets(OctantBucket * bucket)
{
    int numBuck = 8;
    ELink ptr1, ptr2;

    if (bucket->next == nullptr) {
        ptr1 = bucket->lhead;
        while (ptr1 != nullptr) {
            ptr2 = ptr1;
            ptr1 = ptr1->next;
            delete ptr2;
        }
        bucket->list_bb.clear();
        return;
    }

    for (int i = numBuck - 1; i >= 0; i--)
        free_buckets((bucket->next) + i);
    delete[] bucket->next;
}

void
OctTree::destroy()
{
    delete this->info;
    free_buckets(this->root);
    delete this->root;
}

void
OctTree::insert(void * element)
{
    auto bbox = BBFunction(element);
    auto centroid = CentroidFunction(element);
    OctantBucket * bucket = find_element_bucket(this->root, centroid);
    if (bucket)
        add_element_to_bucket(bucket, element, bbox, centroid, this->info);
}

void
OctTree::arrange()
{
    std::vector<void *>::iterator iter;
    for (iter = this->info->list_all_elements.begin(); iter != this->info->list_all_elements.end();
         iter++) {
        auto bbox = BBFunction(*iter);
        insert_one_bb(*iter, bbox, this->root);
    }
    this->info->list_all_elements.clear();
}

void *
OctTree::search(const Point & pt)
{
    return search_element(this->root, pt, this->info);
}

std::vector<void *>
OctTree::search_all(const Point & pt)
{
    return search_all_elements(this->root, pt, this->info);
}

void
OctTree::initialize_octant_buckets(double * orig,
                                   double * size,
                                   int max_elem,
                                   OctantBucket ** buckets_head,
                                   GlobalInfo ** global_para)
{
    int p = 1;
    Point tmp;
    Point error;

    for (int i = 0; i < 3; i++)
        error(i) = size[i] * 0.01;

    // which is a number of 8^p form for integer p
    int initial_buckets_num = (int) pow(8., p); // it is actually 8

    (*global_para) = new GlobalInfo;
    (*global_para)->max_precision = 1;
    (*global_para)->max_elements = max_elem;
    (*global_para)->ptr_to_prev_element = nullptr;

    for (int i = 0; i < 3; i++) {
        (*global_para)->origin[i] = orig[i];
        (*global_para)->size[i] = size[i];
    }

    (*global_para)->num_buckets = initial_buckets_num;
    *buckets_head = new OctantBucket;

    OctantBucket * buckets = new OctantBucket[8];

    (*buckets_head)->next = buckets;
    (*buckets_head)->parent = nullptr;
    (*buckets_head)->num_elements = 0;
    (*buckets_head)->lhead = nullptr;
    (*buckets_head)->precision = 0;
    for (int i = 0; i < 3; i++) {
        (*buckets_head)->min_pt(i) = orig[i] - error(i);
        (*buckets_head)->max_pt(i) = size[i] + orig[i] + error(i);
    }

    for (int i = 0; i < (*global_para)->num_buckets; i++) {
        buckets[i].num_elements = 0;
        buckets[i].lhead = nullptr;
        buckets[i].next = nullptr;
        buckets[i].parent = *buckets_head;
        buckets[i].precision = 1;
    }

    int tmp1 = (int) (pow(2., p));
    for (int i = 0; i < 3; i++) {
        tmp(i) = (double) (size[i] + 2 * error(i)) / tmp1;
    }

    for (int k = 0; k < tmp1; k++) {
        for (int j = 0; j < tmp1; j++) {
            for (int i = 0; i < tmp1; i++) {
                buckets[i + j * tmp1 + k * tmp1 * tmp1].min_pt.x =
                    (*buckets_head)->min_pt.x + tmp.x * i;
                buckets[i + j * tmp1 + k * tmp1 * tmp1].min_pt.y =
                    (*buckets_head)->min_pt.y + tmp.y * j;
                buckets[i + j * tmp1 + k * tmp1 * tmp1].min_pt.z =
                    (*buckets_head)->min_pt.z + tmp.z * k;
                buckets[i + j * tmp1 + k * tmp1 * tmp1].max_pt.x =
                    (*buckets_head)->min_pt.x + tmp.x * (i + 1);
                buckets[i + j * tmp1 + k * tmp1 * tmp1].max_pt.y =
                    (*buckets_head)->min_pt.y + tmp.y * (j + 1);
                buckets[i + j * tmp1 + k * tmp1 * tmp1].max_pt.z =
                    (*buckets_head)->min_pt.z + tmp.z * (k + 1);
            }
        }
    }
}

bool
OctTree::add_element_to_bucket(OctantBucket * bucket,
                               void * element,
                               const BoundingBox3D & bbox,
                               const Point & ele_centroid,
                               GlobalInfo * global_para)
{
    // check for duplicates
    if (check_element_in_bucket(bucket, element))
        return false;

    ELink ptr1 = new Elem;
    global_para->list_all_elements.push_back(element);

    ptr1->next = bucket->lhead;
    ptr1->region = element;
    ptr1->min_pt = bbox.min();
    ptr1->max_pt = bbox.max();
    ptr1->centroid = ele_centroid;

    bucket->lhead = ptr1;
    (bucket->num_elements)++;

    // Check whether the number of elements in the bucket > max_elements
    // if true, refine the bucket and reallocate the elements
    bool flag = true;
    while (flag) {
        flag = false;
        if (bucket->num_elements > global_para->max_elements) {
            subdivide_octant_bucket(bucket, global_para);

            ptr1 = bucket->lhead;
            while (ptr1 != nullptr) {
                OctantBucket * ptrBucket = find_element_bucket(bucket, ptr1->centroid);
                ELink ptr2 = ptr1;
                ptr1 = ptr1->next;
                if (ptrBucket == nullptr) {
                    Log::error("Null bucket in octree");
                    return false;
                }
                ptr2->next = ptrBucket->lhead;
                ptrBucket->lhead = ptr2;
                (ptrBucket->num_elements)++;
                if (ptrBucket->num_elements > global_para->max_elements) {
                    flag = true;
                    bucket->lhead = nullptr;
                    bucket = ptrBucket;
                }
            }
            if (!flag)
                bucket->lhead = nullptr;
        }
    }
    return true;
}

bool
OctTree::check_element_in_bucket(OctantBucket * bucket, void * element)
{
    for (ELink ptr = bucket->lhead; ptr != nullptr; ptr = ptr->next) {
        // changed ****, compare the objected pointed by the void *.
        if (ptr->region == element)
            return true;
    }
    return false;
}

OctTree::OctantBucket *
OctTree::find_element_bucket(OctantBucket * buckets_head, const Point & pt)
{
    int num = 8;
    OctantBucket * prevbucket = nullptr;
    OctantBucket * tmpbucket = buckets_head->next;
    while (tmpbucket != nullptr) {
        int i, j;
        for (i = 0; i < num; i++) {
            for (j = 0; j < 3; j++) {
                if (tmpbucket[i].min_pt(j) > pt(j) || tmpbucket[i].max_pt(j) < pt(j))
                    break;
            }
            if (j == 3) {
                prevbucket = tmpbucket + i;
                tmpbucket = tmpbucket[i].next;
                break;
            }
        }
        if (i == num) {
            Log::error("No bucket contains the given point!");
            return nullptr;
        }
    }
    return prevbucket;
}

int
OctTree::subdivide_octant_bucket(OctantBucket * bucket, GlobalInfo * global_para)
{
    int num_buck = 8;

    bucket->next = new OctantBucket[num_buck];

    global_para->num_buckets += 8;
    if (bucket->precision == global_para->max_precision)
        global_para->max_precision++;
    for (int i = 0; i < num_buck; i++) {
        (bucket->next[i]).num_elements = 0;
        (bucket->next[i]).lhead = nullptr;
        (bucket->next[i]).next = nullptr;
        (bucket->next[i]).parent = bucket;
        (bucket->next[i]).precision = bucket->precision + 1;
    }

    int tmp1 = 2;
    Point tmp;
    for (int i = 0; i < 3; i++) {
        tmp(i) = ((double) (bucket->max_pt(i) - bucket->min_pt(i))) / tmp1;
    }

    for (int k = 0; k < tmp1; k++) {
        for (int j = 0; j < tmp1; j++) {
            for (int i = 0; i < tmp1; i++) {
                bucket->next[i + j * tmp1 + k * tmp1 * tmp1].min_pt.x =
                    bucket->min_pt.x + tmp.x * i;
                bucket->next[i + j * tmp1 + k * tmp1 * tmp1].min_pt.y =
                    bucket->min_pt.y + tmp.y * j;
                bucket->next[i + j * tmp1 + k * tmp1 * tmp1].min_pt.z =
                    bucket->min_pt.z + tmp.z * k;
                bucket->next[i + j * tmp1 + k * tmp1 * tmp1].max_pt.x =
                    bucket->min_pt.x + tmp.x * (i + 1);
                bucket->next[i + j * tmp1 + k * tmp1 * tmp1].max_pt.y =
                    bucket->min_pt.y + tmp.y * (j + 1);
                bucket->next[i + j * tmp1 + k * tmp1 * tmp1].max_pt.z =
                    bucket->min_pt.z + tmp.z * (k + 1);
            }
        }
    }

    return 1;
}

void *
OctTree::search_element(OctantBucket * buckets_head, const Point & pt, GlobalInfo * global_para)
{
    void * ptr_to_ele = nullptr;
#pragma omp atomic read
    ptr_to_ele = global_para->ptr_to_prev_element;

    if (ptr_to_ele) {
        auto flag = xyz_in_element_bb(pt, ptr_to_ele);
        if (flag)
            flag = InEleFunction(ptr_to_ele, pt);
        if (flag)
            return ptr_to_ele;
    }

    OctantBucket * ptr_bucket = find_element_bucket(buckets_head, pt);
    if (ptr_bucket == nullptr) {
        // this is not an error
        // TODO RE ENABLE MSG
        Log::debug("Could not find point in octree (in search element)");
        return nullptr;
    }

    ELink ptr1 = ptr_bucket->lhead;

    while (ptr1 != nullptr) {
        auto flag = xyz_in_element_bb(pt, ptr1->region);
        if (flag)
            flag = InEleFunction(ptr1->region, pt);
        if (flag) {
#pragma omp atomic write
            global_para->ptr_to_prev_element = ptr1->region;
            return ptr1->region;
        }
        ptr1 = ptr1->next;
    }

    for (auto iter = (ptr_bucket->list_bb).begin(); iter != (ptr_bucket->list_bb).end(); iter++) {
        auto flag = xyz_in_element_bb(pt, *iter);
        if (flag)
            flag = InEleFunction(*iter, pt);
        if (flag) {
#pragma omp atomic write
            global_para->ptr_to_prev_element = *iter;
            return *iter;
        }
    }

    // Point is not in the domain
    return nullptr;
}

bool
OctTree::xyz_in_element_bb(const Point & xyz, void * region)
{
    auto bbox = BBFunction(region);
    return bbox.contains(xyz);
}

void
OctTree::insert_one_bb(void * region, const BoundingBox3D & bbox, OctantBucket * bucket)
{
    ELink ptr;
    for (int i = 0; i < 3; i++) {
        if (bucket->min_pt(i) > bbox.max()(i) || bucket->max_pt(i) < bbox.min()(i))
            return;
    }
    if (bucket->next == nullptr) {
        ptr = bucket->lhead;
        while (ptr != nullptr) {
            if (ptr->region == region)
                return;
            ptr = ptr->next;
        }

        bucket->list_bb.push_back(region);
        return;
    }

    for (int i = 0; i < 8; i++)
        insert_one_bb(region, bbox, bucket->next + i);
}

std::vector<void *>
OctTree::search_all_elements(OctantBucket * buckets_head,
                             const Point & pt,
                             GlobalInfo * global_para)
{
    std::vector<void *> elements;
    OctantBucket * ptr_bucket = find_element_bucket(buckets_head, pt);
    if (ptr_bucket == nullptr) {
        Log::debug("Could not find point in octree (in search all elements)");
        return elements;
    }

    ELink ptr1 = ptr_bucket->lhead;
    while (ptr1 != nullptr) {
        auto flag = xyz_in_element_bb(pt, ptr1->region);
        if (flag)
            flag = InEleFunction(ptr1->region, pt);
        if (flag)
            elements.push_back(ptr1->region);
        ptr1 = ptr1->next;
    }

    for (auto iter = (ptr_bucket->list_bb).begin(); iter != (ptr_bucket->list_bb).end(); iter++) {
        auto flag = xyz_in_element_bb(pt, *iter);
        if (flag)
            flag = InEleFunction(*iter, pt);
        if (flag)
            elements.push_back(*iter);
    }

    return elements;
}

} // namespace krado
