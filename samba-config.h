#ifndef SAMABCONFIG_H
#define SAMABCONFIG_H
#include <gio/gio.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _SambaConfigSkeleton             SambaConfigSkeleton;
typedef struct _SambaConfigSkeletonPrivate      SambaConfigSkeletonPrivate;
typedef struct _SambaConfigSkeletonClass        SambaConfigSkeletonClass;

#define SAMBA_CONFIG_TYPE               (samba_config_skeleton_get_type())
#define SAMBA_CONFIG(x)                 (G_TYPE_CHECK_INSTANCE_CAST((x), SAMBA_CONFIG_TYPE, SambaConfigSkeleton))
#define IS_SAMBA_CONFIG(x)              (g_type_instance_type((x), SAMBA_CONFIG_TYPE))


struct _SambaConfigSkeleton
{
    GDBusObjectSkeleton                 parent_instance;
    SambaConfigSkeleton*                priv;
};

struct _SambaConfigSkeletonClass
{
    GDBusObjectSkeletonClass            parent_class;

    bool (*init_authorizations) (SambaConfigSkeleton* pthis, gint64 pid, gint32 uid);
    void (*destory_authorizations) (SambaConfigSkeleton* pthis);
};

GType samba_config_skeleton_get_type (void);

SambaConfigSkeleton* samba_config_skeleton_new (const char* path);


#if GLIB_CHECK_VERSION(2, 44, 0)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(SambaConfigSkeleton, g_object_unref);
#endif


#ifdef __cplusplus
}
#endif

#endif // SAMABCONFIG_H
