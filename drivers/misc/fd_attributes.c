// FD kernel runtime attributes

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/fd_attributes.h>

struct fd_attributes fd_data = {
        .kgsl_skip_zeroing = 0,
};

static struct kobject *fd_kobj;

#define FD_ATTR_RW(name)                                                   \
static ssize_t name##_show(struct kobject *kobj,                           \
                           struct kobj_attribute *attr,                    \
                           char *buf)                                      \
{                                                                          \
        return scnprintf(buf, PAGE_SIZE, "%d\n", READ_ONCE(fd_data.name));          \
}                                                                          \
                                                                            \
static ssize_t name##_store(struct kobject *kobj,                          \
                            struct kobj_attribute *attr,                   \
                            const char *buf, size_t count)                  \
{                                                                          \
        int ret, val;                                                      \
                                                                            \
        ret = kstrtoint(buf, 10, &val);                                    \
        if (ret)                                                           \
                return ret;                                                \
                                                                            \
        if (val != 0 && val != 1)                                          \
                return -EINVAL;                                            \
                                                                            \
        WRITE_ONCE(fd_data.name, val);                                                \
        sysfs_notify(fd_kobj, NULL, #name);                                \
                                                                            \
        return count;                                                      \
}                                                                          \
                                                                            \
static struct kobj_attribute name##_attr =                                 \
        __ATTR(name, 0664, name##_show, name##_store)

FD_ATTR_RW(kgsl_skip_zeroing);

static struct attribute *fd_attrs[] = {
        &kgsl_skip_zeroing_attr.attr,
        NULL,
};

static const struct attribute_group fd_attr_group = {
        .attrs = fd_attrs,
};

static int __init fd_attributes_init(void)
{
        int ret;

        fd_kobj = kobject_create_and_add("fd", kernel_kobj);
        if (!fd_kobj)
                return -ENOMEM;

        ret = sysfs_create_group(fd_kobj, &fd_attr_group);
        if (ret) {
                kobject_put(fd_kobj);
                return ret;
        }

        pr_info("FD: attributes initialized\n");
        return 0;
}

static void __exit fd_attributes_exit(void)
{
        sysfs_remove_group(fd_kobj, &fd_attr_group);
        kobject_put(fd_kobj);
}

core_initcall(fd_attributes_init);
module_exit(fd_attributes_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Fakde Dreamer");
MODULE_DESCRIPTION("FD kernel runtime attributes");
MODULE_VERSION("1.0");
