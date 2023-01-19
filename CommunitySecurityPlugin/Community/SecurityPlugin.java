package Community;
public abstract class SecurityPlugin {
    public static native void Install();

    public static final String LIBRARY_BASE_NAME = "CommunitySecurityPlugin";

    static {
        final String propVal = System.getProperty("opendds.native.debug");
        if ("1".equalsIgnoreCase(propVal) ||
            "y".equalsIgnoreCase(propVal) ||
            "yes".equalsIgnoreCase(propVal) ||
            "t".equalsIgnoreCase(propVal) ||
            "true".equalsIgnoreCase(propVal)) {
            System.loadLibrary(LIBRARY_BASE_NAME + "d");
        } else {
            System.loadLibrary(LIBRARY_BASE_NAME);
        }
    }
}
