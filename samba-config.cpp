#include "samba-config.h"

struct _SambaConfigSkeletonPrivate
{
    GValue*         properties;
    GList*          changedProperties;
    GMutex          lock;
};


static void samba_config_skeleton_finalize (GObject* kclass);
static void samba_config_skeleton_init (SambaConfigSkeleton* skeleton);

#if GLIB_VERSION_MAX_ALLOWED >= GLIB_VERSION_2_38
G_DEFINE_TYPE_WITH_CODE(SambaConfigSkeleton, samba_config_skeleton, G_TYPE_DBUS_OBJECT_SKELETON, G_ADD_PRIVATE(SambaConfigSkeleton))
#else
G_DEFINE_TYPE_WITH_CODE(SambaConfigSkeleton, samba_config_skeleton, G_TYPE_DBUS_OBJECT_SKELETON, G_IMPLEMENT_INTERFACE(SAMBA_CONFIG_TYPE))
#endif


static void samba_config_skeleton_destory_authorizations (SambaConfigSkeleton*);
static bool samba_config_skeleton_init_authorizations (SambaConfigSkeleton*, gint64 pid, gint32 uid);
gboolean samba_config_skeleton_authorize_method (GDBusObjectSkeleton* object, GDBusInterfaceSkeleton* interface, GDBusMethodInvocation* invocation);


static void samba_config_skeleton_init (SambaConfigSkeleton* skeleton)
{

}

static void samba_config_skeleton_class_init (SambaConfigSkeletonClass* kclass)
{
    GObjectClass* gobjectClass = G_OBJECT_CLASS(kclass);

    gobjectClass->finalize = samba_config_skeleton_finalize;

    //
    kclass->init_authorizations = samba_config_skeleton_init_authorizations;
    kclass->destory_authorizations = samba_config_skeleton_destory_authorizations;
}

static void samba_config_skeleton_finalize (GObject* kclass)
{

}

SambaConfigSkeleton *samba_config_skeleton_new(const char* path)
{
    g_return_val_if_fail (g_variant_is_object_path(path), NULL);

    return SAMBA_CONFIG(g_object_new(SAMBA_CONFIG_TYPE, "g-object-path", path, NULL));
}

static void samba_config_skeleton_destory_authorizations (SambaConfigSkeleton* pthis)
{

}

static bool samba_config_skeleton_init_authorizations (SambaConfigSkeleton* pthis, gint64 pid, gint32 uid)
{

    return true;
}


gboolean samba_config_skeleton_authorize_method (GDBusObjectSkeleton* object, GDBusInterfaceSkeleton* interface, GDBusMethodInvocation* invocation)
{
    return true;
}

