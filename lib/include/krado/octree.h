// SPDX-FileCopyrightText: Copyright (C) 1997-2023 C. Geuzaine, J.-F. Remacle
// SPDX-License-Identifier: MIT
//
// This is a re-write of OctTree from GMSH

#pragma once

#include "krado/point.h"
#include "krado/bounding_box_3d.h"
#include <vector>

namespace krado {

class OctTree {
public:
    /// Create
    ///
    /// @param max_elements max. num of elements allowed in an octant
    /// @param bbox model's bounding box
    OctTree(int max_elements, const BoundingBox3D & bbox);

    void destroy();
    void insert(void * element);
    void arrange();
    void * search(const Point & pt);
    std::vector<void *> search_all(const Point & pt);

private:
    // structure for list of elements in an octant
    struct Elem {
        // the pointer to a mesh Db region
        void * region;
        // centroid of element bounding box inside of the octant
        Point centroid;
        // corner of element bounding box nearest the origin
        Point min_pt;
        // corner of elem bound box furthest from the origin
        Point max_pt;
        // link to next item in list, NULL if end
        Elem * next;
    };
    using ELink = Elem *;

    // stucture for octant buckets
    struct OctantBucket {
        //  the point with the smallest coordinates
        Point min_pt;
        //  the point with the biggest coordinates
        Point max_pt;
        // number of elements contained by bucket
        int num_elements;
        // the level of precision of the bucket
        int precision;
        // list of elements in bucket, if NULL -> no elements
        ELink lhead;
        // list of elements in bucket by Bounding Box
        std::vector<void *> list_bb;
        // link to ragged digit extensions to bucket array
        OctantBucket * next;
        // link to the parent bucket
        OctantBucket * parent;
    };

    // structure for global information and requirement
    struct GlobalInfo {
        // number of octant buckets in initial grid array
        int num_buckets;
        // max. number of elements allowed in an octant
        int max_elements;
        // current maximum octant precision for model
        int max_precision;
        // smallest x, y, z of model's bounding box
        double origin[3];
        // size in x, y, z of model bounding box
        double size[3];
        void * ptr_to_prev_element;
        std::vector<void *> list_all_elements;
    };

    virtual BoundingBox3D BBFunction(void *) = 0;
    virtual bool InEleFunction(void *, const Point & pt) = 0;
    virtual Point CentroidFunction(void *) = 0;

    void free_buckets(OctantBucket * bucket);

    void refine_octants(OctantBucket * buckets, GlobalInfo * globalPara);

    /// Add another element to the octant bucket's list.
    /// If the bucket contains too many elements after adding this element,
    /// refine this bucket and reallocate the elements of this bucket
    /// Given:- the octant bucket, - the element
    ///       - the element's minimum and maximum x,y,z
    ///       - the element's centroid,  - global information
    /// Check if element has already been added - if not, return `true`
    /// for successfully adding, otherwise return `false`
    bool add_element_to_bucket(OctantBucket * bucket,
                               void * element,
                               const BoundingBox3D & bbox,
                               const Point & ele_centroid,
                               GlobalInfo * global_para);

    // To many elements are in this octant bucket, so try to refine
    // Returns 1 for success, 0 for failure (no memory left).
    int subdivide_octant_bucket(OctantBucket * bucket, GlobalInfo * global_para);

    /// Initialize the buckets
    /// Given by user: orig and size -- information about the domain
    ///                maxElem -- maximum number of elements per bucket
    /// Return: buckets -- pointer to the begin of buckets
    ///         globalPara -- some info about the buckets
    void initialize_octant_buckets(double * orig,
                                   double * size,
                                   int max_elem,
                                   OctantBucket ** buckets,
                                   GlobalInfo ** global_para);

    /// Given an element and an octant bucket, check if the element exists in the bucket's element
    /// list.
    ///
    /// @return `true` if already exists, otherwise `false`
    bool check_element_in_bucket(OctantBucket * bucket, void * element);

    /// Find the leaf bucket which contains the point `pt`
    ///
    /// @param buckets_head The pointer to buckets head
    /// @param pt The point to find
    /// @return The pointer to the bucket that contains the point, `nullptr` if `pt` not found
    OctantBucket * find_element_bucket(OctantBucket * buckets_head, const Point & pt);

    void * search_element(OctantBucket * buckets_head, const Point & pt, GlobalInfo * global_para);

    /// Check if `xyz` is in the region's bounding box
    bool xyz_in_element_bb(const Point & xyz, void * region);

    void insert_one_bb(void * region, const BoundingBox3D & bbox, OctantBucket * bucket);

    std::vector<void *>
    search_all_elements(OctantBucket * buckets_head, const Point & pt, GlobalInfo * global_para);

    GlobalInfo * info;
    OctantBucket * root;
};

} // namespace krado
