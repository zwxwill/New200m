
/*
 * Copyright (c) 2006-2018 RT-Thread Development Team. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "certs.h"

const char mbedtls_root_certificate[] = 
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\r\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\r\n" \
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\r\n" \
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\r\n" \
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\r\n" \
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\r\n" \
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\r\n" \
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\r\n" \
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\r\n" \
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\r\n" \
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\r\n" \
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\r\n" \
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\r\n" \
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\r\n" \
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\r\n" \
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\r\n" \
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\r\n" \
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\r\n" \
"-----END CERTIFICATE-----\r\n" \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDvTCCAqWgAwIBAgIJAOnBWV5eI6pKMA0GCSqGSIb3DQEBCwUAMHUxCzAJBgNV\r\n" \
"BAYTAmNuMQswCQYDVQQIDAJiajELMAkGA1UEBwwCY3kxDzANBgNVBAoMBmRyYWdv\r\n" \
"bjEMMAoGA1UECwwDend4MQ8wDQYDVQQDDAZvd25fY2ExHDAaBgkqhkiG9w0BCQEW\r\n" \
"DXRlc3RAdGVzdC5jb20wHhcNMTkwODE5MDEwMjA3WhcNMjAwODE4MDEwMjA3WjB1\r\n" \
"MQswCQYDVQQGEwJjbjELMAkGA1UECAwCYmoxCzAJBgNVBAcMAmN5MQ8wDQYDVQQK\r\n" \
"DAZkcmFnb24xDDAKBgNVBAsMA3p3eDEPMA0GA1UEAwwGb3duX2NhMRwwGgYJKoZI\r\n" \
"hvcNAQkBFg10ZXN0QHRlc3QuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIB\r\n" \
"CgKCAQEAwpuvw3A9+aqLun8G0QQL3RuoVYMHr0jjHVf2VEKsRxtM5yviQwcupKJh\r\n" \
"pUPUh/fhh/VKdE+YaBychQUqKTdF/XqzEm5hrl+qXgxeN2CI6ARs97OaK5IVX3h4\r\n" \
"axBb7zD+JHLVbbrt2O/Tsl/Rn+KPTS9Z9vxQpcwNL0DVTa5WKGeIaC356yzxhxti\r\n" \
"6CLN0oDCcQiRwbQM0R0wipz3gGBqKiFk4losAwu3TQWLK/xx5pxLe/euslAMjl94\r\n" \
"+hh9761TBHb10GtoQAuxKx+TXPEUsImfG1dm0N13/MMBzW0xUK/SR8V9d6MvebYU\r\n" \
"AE3RsjCPIgukO2YkwHZKkoh89HezFQIDAQABo1AwTjAdBgNVHQ4EFgQUTiZ9LcRC\r\n" \
"lrQPUqu44tRkdj+K84MwHwYDVR0jBBgwFoAUTiZ9LcRClrQPUqu44tRkdj+K84Mw\r\n" \
"DAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAXATFyS5XRZyj0J8U9sxo\r\n" \
"7reQV/FpD93r5/jM2pdvKCKAsGxr3cyyXeI0dAzRF3ajRthC33UKM4ffRCpwpB96\r\n" \
"TKs4fjQhOFGNLHwNXzFJdxgzXeWXbbelCUtNkEVOuFlGKZXfb7YatWgSYPbr83st\r\n" \
"twLFsa8GopnSrQT1fH9nsD8MDV9cD5ZFFxzg/1fyasyyzxCEq7mn5Quxqkt4Nvis\r\n" \
"rpfSx92Bbplc6g5gaiaZhTYdkiHAEgZuF/8u6UV6hPqqyVMe8iLjKdcWdWRsqXOt\r\n" \
"LOfiTIz+mK6e3OC5OoZs0eL515CMADhjG1E2EN4o5WTT1Vjdl2BDMR92AQxW+ab1\r\n" \
"0Q==\r\n" \
"-----END CERTIFICATE-----\r\n" \

;

const size_t mbedtls_root_certificate_len = sizeof(mbedtls_root_certificate);

