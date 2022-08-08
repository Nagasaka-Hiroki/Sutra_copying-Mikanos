/*
*   USB Descriptorの定義集
*/

#pragma once

#include<cstdint>
#include<array>

namespace usb {
    struct DeviceDescriptor {
        static const uint8_t kType=1;

        uint8_t length;                 // offset 0
        uint8_t descriptor_type;        // offset 1
        uint8_t usb_release;            // offset 2
        uint8_t device_class;           // offset 4
        uint8_t device_sub_class;       // offset 5
        uint8_t device_protocol;        // offset 6
        uint8_t max_packet_size;        // offset 7
        uint16_t vendor_id;             // offset 8
        uint16_t product_id;            // offset 10
        uint16_t device_release;        // offset 12
        uint16_t manufacture;           // offset 14
        uint8_t product;                // offset 15
        uint8_t seria_number;           // offset 16
        uint8_t num_configurations;     // offset 17
    } __attribute__((packed));

    struct ConfigurationDescriptor {
        static const uint8_t kType=2;

        uint8_t length;                 // offset 0
        uint8_t descriptor_type;        // offset 1
        uint16_t total_length;          // offset 2
        uint8_t num_interfaces;         // offset 4
        uint8_t configration_value;     // offset 5
        uint8_t configration_id;        // offset 6
        uint8_t attributes;             // offset 7
        uint8_t max_power;              // offset 8
    } __attribute__((packed));

    struct  InterfaceDescriptor {
        static const uint8_t kType;

        uint8_t length;
        uint8_t descriptor_type;
        uint8_t interface_number;
        uint8_t alternate_setting;
        uint8_t num_endpoints;
        uint8_t interface_class;
        uint8_t interface_sub_class;
        uint8_t interface_protocol;
        uint8_t interface_id;
    } __attribute__((packed));

    struct  EndPointDescripor {
        static const uint8_t kType=5;

        uint8_t length;
        uint8_t descriptor_type;
        union {
            uint8_t data;
            struct {
                uint8_t number : 4;
                uint8_t : 3;
                uint8_t dir_in : 3;
            } __attribute__((packed)) bits;
        } endpointaddress;
        union {
            uint8_t data;
            struct {
                uint8_t transfer_type : 2;
                uint8_t sync_type : 2;
                uint8_t usage_type : 2;
                uint8_t : 2;
            } __attribute__((packed)) bits;

        } attributes;
        uint16_t max_packet_size;
        uint8_t interval;
    } __attribute__((packed));

    struct  HIDDescriptor{
        static const uint8_t kType=33;

        uint8_t length;
        uint8_t descriptor_type;
        uint16_t hid_release;
        uint8_t country_code;
        uint8_t num_descriptors;

        struct ClassDescriptor {
            uint8_t descriptor_type;
            uint16_t descriptor_lenght;
        } __attribute__((packed));

        ClassDescriptor* GetClassDescriptor(size_t index) const {
            if(index >= num_descriptors) {
                return nullptr;
            }
            const auto end_of_struct = reinterpret_cast<uintptr_t>(this) + sizeof(HIDDescriptor);
            return reinterpret_cast<ClassDescriptor*>(end_of_struct) + index;
        }
    } __attribute__((packed));

    template <class T>
    T* DescriptorDynamicCast(uint8_t* desc_data) {
        if(desc_data[1]==T::kType){
            return reinterpret_cast<T*>(desc_data);
        }
        return nullptr;
    }

    template <class T>
    const T* DescriptorDynamicCast(const uint8_t* desc_data) {
        if(desc_data[1] == T::kType){
            return reinterpret_cast<const T*> (desc_data);
        }
        return nullptr;
    }
}