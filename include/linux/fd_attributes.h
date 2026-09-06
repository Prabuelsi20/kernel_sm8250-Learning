#ifndef _FD_ATTRIBUTES_H
#define _FD_ATTRIBUTES_H

struct fd_attributes {
        int kgsl_skip_zeroing;
        int avoid_dirty_pte;
};

extern struct fd_attributes fd_data;

#endif /* _FD_ATTRIBUTES_H */
