#include "CommunitySecurityPlugin.h"

#include <dds/DCPS/security/AccessControlBuiltInImpl.h>
#include <dds/DCPS/security/AuthenticationBuiltInImpl.h>
#include <dds/DCPS/security/CommonUtilities.h>
#include <dds/DCPS/security/CryptoBuiltInImpl.h>
#include <dds/DCPS/security/UtilityImpl.h>
#include <dds/DCPS/security/framework/SecurityConfig.h>
#include <dds/DCPS/security/framework/SecurityPluginInst.h>
#include <dds/DCPS/security/framework/SecurityPluginInst_rch.h>
#include <dds/DCPS/security/framework/SecurityRegistry.h>


namespace Community {

class AccessControl : public OpenDDS::Security::AccessControlBuiltInImpl {
public:
  bool check_local_datawriter_register_instance(DDS::Security::PermissionsHandle permissions_handle,
                                                DDS::DataWriter_ptr writer,
                                                DDS::DynamicData_ptr key,
                                                DDS::Security::SecurityException& ex)
  {
    return OpenDDS::Security::AccessControlBuiltInImpl::check_local_datawriter_register_instance(permissions_handle, writer, key, ex) && common_check(permissions_handle, key, ex);
  }

  bool check_local_datawriter_dispose_instance(DDS::Security::PermissionsHandle permissions_handle,
                                               DDS::DataWriter_ptr writer,
                                               DDS::DynamicData_ptr key,
                                               DDS::Security::SecurityException& ex)
  {
    return OpenDDS::Security::AccessControlBuiltInImpl::check_local_datawriter_dispose_instance(permissions_handle, writer, key, ex) && common_check(permissions_handle, key, ex);
  }

  bool check_remote_datawriter_register_instance(DDS::Security::PermissionsHandle permissions_handle,
                                                 DDS::DataReader_ptr reader,
                                                 DDS::InstanceHandle_t publication_handle,
                                                 DDS::DynamicData_ptr key,
                                                 DDS::Security::SecurityException& ex)
  {
    return OpenDDS::Security::AccessControlBuiltInImpl::check_remote_datawriter_register_instance(permissions_handle, reader, publication_handle, key, ex) && common_check(permissions_handle, key, ex);
  }

  bool check_remote_datawriter_dispose_instance(DDS::Security::PermissionsHandle permissions_handle,
                                                DDS::DataReader_ptr reader,
                                                DDS::InstanceHandle_t publication_handle,
                                                DDS::DynamicData_ptr key,
                                                DDS::Security::SecurityException& ex)
  {
    return OpenDDS::Security::AccessControlBuiltInImpl::check_remote_datawriter_dispose_instance(permissions_handle, reader, publication_handle, key, ex) && common_check(permissions_handle, key, ex);
  }

private:
  bool common_check(DDS::Security::PermissionsHandle permissions_handle,
                    DDS::DynamicData_ptr key,
                    DDS::Security::SecurityException& ex) const
  {
    DDS::DynamicType_var type = key->type();
    if (0 == type) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: No type support");
    }

    DDS::DynamicTypeMember_var dtm;
    if (type->get_member_by_name(dtm, "dpmgid") != DDS::RETCODE_OK) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: No dpmgid in type");
    }

    DDS::MemberDescriptor_var md;
    if (dtm->get_descriptor(md) != DDS::RETCODE_OK) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: No type descriptor");
    }

    if (!md->is_key()) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: dpmgid is not a key");
    }

    CORBA::String_var dpmgid;
    if (key->get_string_value(dpmgid, md->id()) != DDS::RETCODE_OK) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: Could not get dpmgid");
    }

    const OpenDDS::Security::SSL::SubjectName sn = get_subject_name(permissions_handle);
    const OpenDDS::Security::SSL::SubjectName::const_iterator pos = sn.find("SN");
    if (pos == sn.end()) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: Subject name does not contain a SN");
    }

    if (pos->second != dpmgid.in()) {
      return OpenDDS::Security::CommonUtilities::set_security_error(ex, -1, 0, "CommunityAccessControl::common_check: dpmgid from sample does not match SN from subject name");
    }

    return true;
  }
};

class SecurityPluginInst : public OpenDDS::Security::SecurityPluginInst {
public:
  SecurityPluginInst()
    : authentication_(new OpenDDS::Security::AuthenticationBuiltInImpl)
    , access_control_(new AccessControl)
    , key_factory_(new OpenDDS::Security::CryptoBuiltInImpl)
    , key_exchange_(DDS::Security::CryptoKeyExchange::_narrow(key_factory_))
    , transform_(DDS::Security::CryptoTransform::_narrow(key_factory_))
    , utility_(OpenDDS::DCPS::make_rch<OpenDDS::Security::UtilityImpl>())
  {}

private:
  DDS::Security::Authentication_var create_authentication() { return authentication_; }
  DDS::Security::AccessControl_var create_access_control() { return access_control_; }
  DDS::Security::CryptoKeyExchange_var create_crypto_key_exchange() { return key_exchange_; }
  DDS::Security::CryptoKeyFactory_var create_crypto_key_factory() { return key_factory_; }
  DDS::Security::CryptoTransform_var create_crypto_transform() { return transform_; }
  OpenDDS::DCPS::RcHandle<OpenDDS::Security::Utility> create_utility() { return utility_; }
  void shutdown() {}

  DDS::Security::Authentication_var authentication_;
  DDS::Security::AccessControl_var access_control_;
  DDS::Security::CryptoKeyFactory_var key_factory_;
  DDS::Security::CryptoKeyExchange_var key_exchange_;
  DDS::Security::CryptoTransform_var transform_;
  OpenDDS::DCPS::RcHandle<OpenDDS::Security::Utility> utility_;
};

void install_community_security_plugin()
{
  OpenDDS::DCPS::RcHandle<SecurityPluginInst> community_plugin = OpenDDS::DCPS::make_rch<SecurityPluginInst>();
  TheSecurityRegistry->register_plugin("Community", community_plugin);
  OpenDDS::Security::SecurityConfig_rch community_config = TheSecurityRegistry->create_config("Community", community_plugin);
  TheSecurityRegistry->default_config(community_config);
}

}

#ifdef COMMUNITYSECURITYPLUGIN_HAS_JAVA
#include "idl2jni_jni.h"

JNIEXPORT void JNICALL Java_Community_SecurityPlugin_Install(JNIEnv*, jclass)
{
  Community::install_community_security_plugin();
}
#endif
