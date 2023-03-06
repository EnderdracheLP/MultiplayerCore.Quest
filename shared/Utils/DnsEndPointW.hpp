#include "main.hpp"
#include "GlobalNamespace/DnsEndPoint.hpp"

namespace MultiplayerCore {
    struct DnsEndPointW {
        StringW hostName;
        int port;
        GlobalNamespace::DnsEndPoint* _dnsEndPoint;
        constexpr DnsEndPointW() noexcept {
            this->Clear();
            InitDnsEndpoint();
        }
        // constexpr DnsEndPointW() noexcept : _dnsEndPoint(nullptr) {}
        constexpr operator GlobalNamespace::DnsEndPoint*() const {
            if (_dnsEndPoint)
                return _dnsEndPoint;
            return GlobalNamespace::DnsEndPoint::New_ctor<il2cpp_utils::CreationType::Temporary>(hostName, port);
        }
        constexpr GlobalNamespace::DnsEndPoint* operator->() noexcept {
            if (_dnsEndPoint)
                return _dnsEndPoint;
            return GlobalNamespace::DnsEndPoint::New_ctor<il2cpp_utils::CreationType::Temporary>(hostName, port);
        }
        constexpr operator bool() const noexcept {
            return _dnsEndPoint && _dnsEndPoint->hostName && !Il2CppString::IsNullOrEmpty(_dnsEndPoint->hostName) && _dnsEndPoint->port > 0;
        }
        inline DnsEndPointW& operator=(GlobalNamespace::DnsEndPoint* other) {
            if (other != nullptr && other->hostName && other->port != 0 &&
                (other->hostName != _dnsEndPoint->hostName || other->port != _dnsEndPoint->port))
            {
                this->Clear();
                // _dnsEndPoint = other;
                if (_dnsEndPoint)
                {
                    _dnsEndPoint->hostName = other->hostName;
                    _dnsEndPoint->port = other->port;
                    _dnsEndPoint->getEndPointTask = other->getEndPointTask;
                } else {
                    InitDnsEndpoint(other->hostName, other->port);
                }
                // this->InitDnsEndpoint();
                // this->_dnsEndPoint = GlobalNamespace::DnsEndPoint::New_ctor<il2cpp_utils::CreationType::Manual>(other->hostName, other->port);
            }
            return *this;
        }
        inline DnsEndPointW& operator=(DnsEndPointW& other) {
            this->~DnsEndPointW();
            if (other._dnsEndPoint) this->_dnsEndPoint = other._dnsEndPoint;
            else this->InitDnsEndpoint(other.hostName, other.port);
            return *this;
        }
        StringW ToString() const {
            return StringW(this->ToCPPString());
        }
        std::string ToCPPString() const {
            return string_format("%s:%d", static_cast<std::string>(_dnsEndPoint->hostName).c_str(), _dnsEndPoint->port);
        }
        operator std::string() const {
            return ToCPPString();
        }
        void Clear() {
            if (_dnsEndPoint && _dnsEndPoint->getEndPointTask) {
                gc_free_specific(_dnsEndPoint->getEndPointTask);
                _dnsEndPoint->getEndPointTask = nullptr;
                _dnsEndPoint->hostName = nullptr;
                _dnsEndPoint->port = 0;
            }
            // hostName = nullptr;
            // port = 0;
        }
        bool operator ==(DnsEndPointW rhs) const noexcept {
            return ToCPPString() == rhs.ToCPPString();
        }
        bool operator ==(GlobalNamespace::DnsEndPoint* rhs) const noexcept {
            return ToCPPString() == static_cast<std::string>(rhs->ToString());
        }
        void InitDnsEndpoint(StringW hostName = nullptr, int port = 0) {
            this->hostName = hostName;
            this->port = port;
            if (il2cpp_functions::initialized) {
                if (!classof(GlobalNamespace::DnsEndPoint*)->initialized) il2cpp_functions::Class_Init(classof(GlobalNamespace::DnsEndPoint*)); // This is needed in order to initialize the Il2CppClass
                this->_dnsEndPoint = GlobalNamespace::DnsEndPoint::New_ctor<il2cpp_utils::CreationType::Manual>(hostName, port);
            }
        }
        constexpr DnsEndPointW(StringW host, int port) {
            InitDnsEndpoint(host, port);
        }
        constexpr DnsEndPointW(GlobalNamespace::DnsEndPoint* dnsEndPoint) {
            InitDnsEndpoint(dnsEndPoint->hostName, dnsEndPoint->port);
        }
        constexpr ~DnsEndPointW() {
            Clear();
            if (_dnsEndPoint) gc_free_specific(_dnsEndPoint);
            _dnsEndPoint = nullptr;
        }
    // private:
    //     GlobalNamespace::DnsEndPoint* _dnsEndPoint;
    };
}