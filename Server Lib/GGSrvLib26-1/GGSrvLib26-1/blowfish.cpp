// Arquivo blowfish.cpp
// Criado em 25/03/2022 as 22:52 por Acrisio
// Implementação do algoritimo de encriptação blowfish
// Cópia com pequenas modificações do código fonte em c do Bruce Schneier
// fonte: https://www.schneier.com/academic/blowfish/

#ifdef little_endian   /* Eg: Intel */
#include <dos.h>
#include <graphics.h>
#include <io.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef little_endian   /* Eg: Intel */
#include <alloc.h>
#endif

#include <ctype.h>

#ifdef little_endian   /* Eg: Intel */
#include <dir.h>
#include <bios.h>
#endif

#ifdef big_endian
#include <Types.h>
#endif

#include "blowfish.h"

#include <memory.h>

constexpr unsigned char gP[] = { 
	0x88, 0x6a, 0x3f, 0x24, 0xd3, 0x08, 0xa3, 0x85, 0x2e, 0x8a, 0x19, 0x13, 0x44, 0x73, 0x70, 0x03, 
	0x22, 0x38, 0x09, 0xa4, 0xd0, 0x31, 0x9f, 0x29, 0x98, 0xfa, 0x2e, 0x08, 0x89, 0x6c, 0x4e, 0xec, 
	0xe6, 0x21, 0x28, 0x45, 0x77, 0x13, 0xd0, 0x38, 0xcf, 0x66, 0x54, 0xbe, 0x6c, 0x0c, 0xe9, 0x34, 
	0xb7, 0x29, 0xac, 0xc0, 0xdd, 0x50, 0x7c, 0xc9, 0xb5, 0xd5, 0x84, 0x3f, 0x17, 0x09, 0x47, 0xb5, 
	0xd9, 0xd5, 0x16, 0x92, 0x1b, 0xfb, 0x79, 0x89
};

constexpr unsigned char gBoxS[] = { 
		0xa6, 0x0b, 0x31, 0xd1, 0xac, 0xb5, 0xdf, 0x98, 0xdb, 0x72, 0xfd, 0x2f, 0xb7, 0xdf, 0x1a, 0xd0, 0xed, 0xaf, 0xe1, 0xb8, 0x96, 0x7e, 0x26, 0x6a, 0x45, 0x90, 0x7c, 0xba, 0x99, 0x7f, 0x2c, 0xf1,
		0x47, 0x99, 0xa1, 0x24, 0xf7, 0x6c, 0x91, 0xb3, 0xe2, 0xf2, 0x01, 0x08, 0x16, 0xfc, 0x8e, 0x85, 0xd8, 0x20, 0x69, 0x63, 0x69, 0x4e, 0x57, 0x71, 0xa3, 0xfe, 0x58, 0xa4, 0x7e, 0x3d, 0x93, 0xf4, 
		0x8f, 0x74, 0x95, 0x0d, 0x58, 0xb6, 0x8e, 0x72, 0x58, 0xcd, 0x8b, 0x71, 0xee, 0x4a, 0x15, 0x82, 0x1d, 0xa4, 0x54, 0x7b, 0xb5, 0x59, 0x5a, 0xc2, 0x39, 0xd5, 0x30, 0x9c, 0x13, 0x60, 0xf2, 0x2a, 
		0x23, 0xb0, 0xd1, 0xc5, 0xf0, 0x85, 0x60, 0x28, 0x18, 0x79, 0x41, 0xca, 0xef, 0x38, 0xdb, 0xb8, 0xb0, 0xdc, 0x79, 0x8e, 0x0e, 0x18, 0x3a, 0x60, 0x8b, 0x0e, 0x9e, 0x6c, 0x3e, 0x8a, 0x1e, 0xb0, 
		0xc1, 0x77, 0x15, 0xd7, 0x27, 0x4b, 0x31, 0xbd, 0xda, 0x2f, 0xaf, 0x78, 0x60, 0x5c, 0x60, 0x55, 0xf3, 0x25, 0x55, 0xe6, 0x94, 0xab, 0x55, 0xaa, 0x62, 0x98, 0x48, 0x57, 0x40, 0x14, 0xe8, 0x63, 
		0x6a, 0x39, 0xca, 0x55, 0xb6, 0x10, 0xab, 0x2a, 0x34, 0x5c, 0xcc, 0xb4, 0xce, 0xe8, 0x41, 0x11, 0xaf, 0x86, 0x54, 0xa1, 0x93, 0xe9, 0x72, 0x7c, 0x11, 0x14, 0xee, 0xb3, 0x2a, 0xbc, 0x6f, 0x63, 
		0x5d, 0xc5, 0xa9, 0x2b, 0xf6, 0x31, 0x18, 0x74, 0x16, 0x3e, 0x5c, 0xce, 0x1e, 0x93, 0x87, 0x9b, 0x33, 0xba, 0xd6, 0xaf, 0x5c, 0xcf, 0x24, 0x6c, 0x81, 0x53, 0x32, 0x7a, 0x77, 0x86, 0x95, 0x28, 
		0x98, 0x48, 0x8f, 0x3b, 0xaf, 0xb9, 0x4b, 0x6b, 0x1b, 0xe8, 0xbf, 0xc4, 0x93, 0x21, 0x28, 0x66, 0xcc, 0x09, 0xd8, 0x61, 0x91, 0xa9, 0x21, 0xfb, 0x60, 0xac, 0x7c, 0x48, 0x32, 0x80, 0xec, 0x5d, 
		0x5d, 0x5d, 0x84, 0xef, 0xb1, 0x75, 0x85, 0xe9, 0x02, 0x23, 0x26, 0xdc, 0x88, 0x1b, 0x65, 0xeb, 0x81, 0x3e, 0x89, 0x23, 0xc5, 0xac, 0x96, 0xd3, 0xf3, 0x6f, 0x6d, 0x0f, 0x39, 0x42, 0xf4, 0x83, 
		0x82, 0x44, 0x0b, 0x2e, 0x04, 0x20, 0x84, 0xa4, 0x4a, 0xf0, 0xc8, 0x69, 0x5e, 0x9b, 0x1f, 0x9e, 0x42, 0x68, 0xc6, 0x21, 0x9a, 0x6c, 0xe9, 0xf6, 0x61, 0x9c, 0x0c, 0x67, 0xf0, 0x88, 0xd3, 0xab, 
		0xd2, 0xa0, 0x51, 0x6a, 0x68, 0x2f, 0x54, 0xd8, 0x28, 0xa7, 0x0f, 0x96, 0xa3, 0x33, 0x51, 0xab, 0x6c, 0x0b, 0xef, 0x6e, 0xe4, 0x3b, 0x7a, 0x13, 0x50, 0xf0, 0x3b, 0xba, 0x98, 0x2a, 0xfb, 0x7e, 
		0x1d, 0x65, 0xf1, 0xa1, 0x76, 0x01, 0xaf, 0x39, 0x3e, 0x59, 0xca, 0x66, 0x88, 0x0e, 0x43, 0x82, 0x19, 0x86, 0xee, 0x8c, 0xb4, 0x9f, 0x6f, 0x45, 0xc3, 0xa5, 0x84, 0x7d, 0xbe, 0x5e, 0x8b, 0x3b, 
		0xd8, 0x75, 0x6f, 0xe0, 0x73, 0x20, 0xc1, 0x85, 0x9f, 0x44, 0x1a, 0x40, 0xa6, 0x6a, 0xc1, 0x56, 0x62, 0xaa, 0xd3, 0x4e, 0x06, 0x77, 0x3f, 0x36, 0x72, 0xdf, 0xfe, 0x1b, 0x3d, 0x02, 0x9b, 0x42, 
		0x24, 0xd7, 0xd0, 0x37, 0x48, 0x12, 0x0a, 0xd0, 0xd3, 0xea, 0x0f, 0xdb, 0x9b, 0xc0, 0xf1, 0x49, 0xc9, 0x72, 0x53, 0x07, 0x7b, 0x1b, 0x99, 0x80, 0xd8, 0x79, 0xd4, 0x25, 0xf7, 0xde, 0xe8, 0xf6, 
		0x1a, 0x50, 0xfe, 0xe3, 0x3b, 0x4c, 0x79, 0xb6, 0xbd, 0xe0, 0x6c, 0x97, 0xba, 0x06, 0xc0, 0x04, 0xb6, 0x4f, 0xa9, 0xc1, 0xc4, 0x60, 0x9f, 0x40, 0xc2, 0x9e, 0x5c, 0x5e, 0x63, 0x24, 0x6a, 0x19, 
		0xaf, 0x6f, 0xfb, 0x68, 0xb5, 0x53, 0x6c, 0x3e, 0xeb, 0xb2, 0x39, 0x13, 0x6f, 0xec, 0x52, 0x3b, 0x1f, 0x51, 0xfc, 0x6d, 0x2c, 0x95, 0x30, 0x9b, 0x44, 0x45, 0x81, 0xcc, 0x09, 0xbd, 0x5e, 0xaf, 
		0x04, 0xd0, 0xe3, 0xbe, 0xfd, 0x4a, 0x33, 0xde, 0x07, 0x28, 0x0f, 0x66, 0xb3, 0x4b, 0x2e, 0x19, 0x57, 0xa8, 0xcb, 0xc0, 0x0f, 0x74, 0xc8, 0x45, 0x39, 0x5f, 0x0b, 0xd2, 0xdb, 0xfb, 0xd3, 0xb9, 
		0xbd, 0xc0, 0x79, 0x55, 0x0a, 0x32, 0x60, 0x1a, 0xc6, 0x00, 0xa1, 0xd6, 0x79, 0x72, 0x2c, 0x40, 0xfe, 0x25, 0x9f, 0x67, 0xcc, 0xa3, 0x1f, 0xfb, 0xf8, 0xe9, 0xa5, 0x8e, 0xf8, 0x22, 0x32, 0xdb, 
		0xdf, 0x16, 0x75, 0x3c, 0x15, 0x6b, 0x61, 0xfd, 0xc8, 0x1e, 0x50, 0x2f, 0xab, 0x52, 0x05, 0xad, 0xfa, 0xb5, 0x3d, 0x32, 0x60, 0x87, 0x23, 0xfd, 0x48, 0x7b, 0x31, 0x53, 0x82, 0xdf, 0x00, 0x3e, 
		0xbb, 0x57, 0x5c, 0x9e, 0xa0, 0x8c, 0x6f, 0xca, 0x2e, 0x56, 0x87, 0x1a, 0xdb, 0x69, 0x17, 0xdf, 0xf6, 0xa8, 0x42, 0xd5, 0xc3, 0xff, 0x7e, 0x28, 0xc6, 0x32, 0x67, 0xac, 0x73, 0x55, 0x4f, 0x8c, 
		0xb0, 0x27, 0x5b, 0x69, 0xc8, 0x58, 0xca, 0xbb, 0x5d, 0xa3, 0xff, 0xe1, 0xa0, 0x11, 0xf0, 0xb8, 0x98, 0x3d, 0xfa, 0x10, 0xb8, 0x83, 0x21, 0xfd, 0x6c, 0xb5, 0xfc, 0x4a, 0x5b, 0xd3, 0xd1, 0x2d, 
		0x79, 0xe4, 0x53, 0x9a, 0x65, 0x45, 0xf8, 0xb6, 0xbc, 0x49, 0x8e, 0xd2, 0x90, 0x97, 0xfb, 0x4b, 0xda, 0xf2, 0xdd, 0xe1, 0x33, 0x7e, 0xcb, 0xa4, 0x41, 0x13, 0xfb, 0x62, 0xe8, 0xc6, 0xe4, 0xce, 
		0xda, 0xca, 0x20, 0xef, 0x01, 0x4c, 0x77, 0x36, 0xfe, 0x9e, 0x7e, 0xd0, 0xb4, 0x1f, 0xf1, 0x2b, 0x4d, 0xda, 0xdb, 0x95, 0x98, 0x91, 0x90, 0xae, 0x71, 0x8e, 0xad, 0xea, 0xa0, 0xd5, 0x93, 0x6b, 
		0xd0, 0xd1, 0x8e, 0xd0, 0xe0, 0x25, 0xc7, 0xaf, 0x2f, 0x5b, 0x3c, 0x8e, 0xb7, 0x94, 0x75, 0x8e, 0xfb, 0xe2, 0xf6, 0x8f, 0x64, 0x2b, 0x12, 0xf2, 0x12, 0xb8, 0x88, 0x88, 0x1c, 0xf0, 0x0d, 0x90, 
		0xa0, 0x5e, 0xad, 0x4f, 0x1c, 0xc3, 0x8f, 0x68, 0x91, 0xf1, 0xcf, 0xd1, 0xad, 0xc1, 0xa8, 0xb3, 0x18, 0x22, 0x2f, 0x2f, 0x77, 0x17, 0x0e, 0xbe, 0xfe, 0x2d, 0x75, 0xea, 0xa1, 0x1f, 0x02, 0x8b, 
		0x0f, 0xcc, 0xa0, 0xe5, 0xe8, 0x74, 0x6f, 0xb5, 0xd6, 0xf3, 0xac, 0x18, 0x99, 0xe2, 0x89, 0xce, 0xe0, 0x4f, 0xa8, 0xb4, 0xb7, 0xe0, 0x13, 0xfd, 0x81, 0x3b, 0xc4, 0x7c, 0xd9, 0xa8, 0xad, 0xd2, 
		0x66, 0xa2, 0x5f, 0x16, 0x05, 0x77, 0x95, 0x80, 0x14, 0x73, 0xcc, 0x93, 0x77, 0x14, 0x1a, 0x21, 0x65, 0x20, 0xad, 0xe6, 0x86, 0xfa, 0xb5, 0x77, 0xf5, 0x42, 0x54, 0xc7, 0xcf, 0x35, 0x9d, 0xfb, 
		0x0c, 0xaf, 0xcd, 0xeb, 0xa0, 0x89, 0x3e, 0x7b, 0xd3, 0x1b, 0x41, 0xd6, 0x49, 0x7e, 0x1e, 0xae, 0x2d, 0x0e, 0x25, 0x00, 0x5e, 0xb3, 0x71, 0x20, 0xbb, 0x00, 0x68, 0x22, 0xaf, 0xe0, 0xb8, 0x57, 
		0x9b, 0x36, 0x64, 0x24, 0x1e, 0xb9, 0x09, 0xf0, 0x1d, 0x91, 0x63, 0x55, 0xaa, 0xa6, 0xdf, 0x59, 0x89, 0x43, 0xc1, 0x78, 0x7f, 0x53, 0x5a, 0xd9, 0xa2, 0x5b, 0x7d, 0x20, 0xc5, 0xb9, 0xe5, 0x02, 
		0x76, 0x03, 0x26, 0x83, 0xa9, 0xcf, 0x95, 0x62, 0x68, 0x19, 0xc8, 0x11, 0x41, 0x4a, 0x73, 0x4e, 0xca, 0x2d, 0x47, 0xb3, 0x4a, 0xa9, 0x14, 0x7b, 0x52, 0x00, 0x51, 0x1b, 0x15, 0x29, 0x53, 0x9a, 
		0x3f, 0x57, 0x0f, 0xd6, 0xe4, 0xc6, 0x9b, 0xbc, 0x76, 0xa4, 0x60, 0x2b, 0x00, 0x74, 0xe6, 0x81, 0xb5, 0x6f, 0xba, 0x08, 0x1f, 0xe9, 0x1b, 0x57, 0x6b, 0xec, 0x96, 0xf2, 0x15, 0xd9, 0x0d, 0x2a, 
		0x21, 0x65, 0x63, 0xb6, 0xb6, 0xf9, 0xb9, 0xe7, 0x2e, 0x05, 0x34, 0xff, 0x64, 0x56, 0x85, 0xc5, 0x5d, 0x2d, 0xb0, 0x53, 0xa1, 0x8f, 0x9f, 0xa9, 0x99, 0x47, 0xba, 0x08, 0x6a, 0x07, 0x85, 0x6e, 
		0xe9, 0x70, 0x7a, 0x4b, 0x44, 0x29, 0xb3, 0xb5, 0x2e, 0x09, 0x75, 0xdb, 0x23, 0x26, 0x19, 0xc4, 0xb0, 0xa6, 0x6e, 0xad, 0x7d, 0xdf, 0xa7, 0x49, 0xb8, 0x60, 0xee, 0x9c, 0x66, 0xb2, 0xed, 0x8f, 
		0x71, 0x8c, 0xaa, 0xec, 0xff, 0x17, 0x9a, 0x69, 0x6c, 0x52, 0x64, 0x56, 0xe1, 0x9e, 0xb1, 0xc2, 0xa5, 0x02, 0x36, 0x19, 0x29, 0x4c, 0x09, 0x75, 0x40, 0x13, 0x59, 0xa0, 0x3e, 0x3a, 0x18, 0xe4, 
		0x9a, 0x98, 0x54, 0x3f, 0x65, 0x9d, 0x42, 0x5b, 0xd6, 0xe4, 0x8f, 0x6b, 0xd6, 0x3f, 0xf7, 0x99, 0x07, 0x9c, 0xd2, 0xa1, 0xf5, 0x30, 0xe8, 0xef, 0xe6, 0x38, 0x2d, 0x4d, 0xc1, 0x5d, 0x25, 0xf0, 
		0x86, 0x20, 0xdd, 0x4c, 0x26, 0xeb, 0x70, 0x84, 0xc6, 0xe9, 0x82, 0x63, 0x5e, 0xcc, 0x1e, 0x02, 0x3f, 0x6b, 0x68, 0x09, 0xc9, 0xef, 0xba, 0x3e, 0x14, 0x18, 0x97, 0x3c, 0xa1, 0x70, 0x6a, 0x6b, 
		0x84, 0x35, 0x7f, 0x68, 0x86, 0xe2, 0xa0, 0x52, 0x05, 0x53, 0x9c, 0xb7, 0x37, 0x07, 0x50, 0xaa, 0x1c, 0x84, 0x07, 0x3e, 0x5c, 0xae, 0xde, 0x7f, 0xec, 0x44, 0x7d, 0x8e, 0xb8, 0xf2, 0x16, 0x57, 
		0x37, 0xda, 0x3a, 0xb0, 0x0d, 0x0c, 0x50, 0xf0, 0x04, 0x1f, 0x1c, 0xf0, 0xff, 0xb3, 0x00, 0x02, 0x1a, 0xf5, 0x0c, 0xae, 0xb2, 0x74, 0xb5, 0x3c, 0x58, 0x7a, 0x83, 0x25, 0xbd, 0x21, 0x09, 0xdc, 
		0xf9, 0x13, 0x91, 0xd1, 0xf6, 0x2f, 0xa9, 0x7c, 0x73, 0x47, 0x32, 0x94, 0x01, 0x47, 0xf5, 0x22, 0x81, 0xe5, 0xe5, 0x3a, 0xdc, 0xda, 0xc2, 0x37, 0x34, 0x76, 0xb5, 0xc8, 0xa7, 0xdd, 0xf3, 0x9a, 
		0x46, 0x61, 0x44, 0xa9, 0x0e, 0x03, 0xd0, 0x0f, 0x3e, 0xc7, 0xc8, 0xec, 0x41, 0x1e, 0x75, 0xa4, 0x99, 0xcd, 0x38, 0xe2, 0x2f, 0x0e, 0xea, 0x3b, 0xa1, 0xbb, 0x80, 0x32, 0x31, 0xb3, 0x3e, 0x18, 
		0x38, 0x8b, 0x54, 0x4e, 0x08, 0xb9, 0x6d, 0x4f, 0x03, 0x0d, 0x42, 0x6f, 0xbf, 0x04, 0x0a, 0xf6, 0x90, 0x12, 0xb8, 0x2c, 0x79, 0x7c, 0x97, 0x24, 0x72, 0xb0, 0x79, 0x56, 0xaf, 0x89, 0xaf, 0xbc, 
		0x1f, 0x77, 0x9a, 0xde, 0x10, 0x08, 0x93, 0xd9, 0x12, 0xae, 0x8b, 0xb3, 0x2e, 0x3f, 0xcf, 0xdc, 0x1f, 0x72, 0x12, 0x55, 0x24, 0x71, 0x6b, 0x2e, 0xe6, 0xdd, 0x1a, 0x50, 0x87, 0xcd, 0x84, 0x9f, 
		0x18, 0x47, 0x58, 0x7a, 0x17, 0xda, 0x08, 0x74, 0xbc, 0x9a, 0x9f, 0xbc, 0x8c, 0x7d, 0x4b, 0xe9, 0x3a, 0xec, 0x7a, 0xec, 0xfa, 0x1d, 0x85, 0xdb, 0x66, 0x43, 0x09, 0x63, 0xd2, 0xc3, 0x64, 0xc4, 
		0x47, 0x18, 0x1c, 0xef, 0x08, 0xd9, 0x15, 0x32, 0x37, 0x3b, 0x43, 0xdd, 0x16, 0xba, 0xc2, 0x24, 0x43, 0x4d, 0xa1, 0x12, 0x51, 0xc4, 0x65, 0x2a, 0x02, 0x00, 0x94, 0x50, 0xdd, 0xe4, 0x3a, 0x13, 
		0x9e, 0xf8, 0xdf, 0x71, 0x55, 0x4e, 0x31, 0x10, 0xd6, 0x77, 0xac, 0x81, 0x9b, 0x19, 0x11, 0x5f, 0xf1, 0x56, 0x35, 0x04, 0x6b, 0xc7, 0xa3, 0xd7, 0x3b, 0x18, 0x11, 0x3c, 0x09, 0xa5, 0x24, 0x59, 
		0xed, 0xe6, 0x8f, 0xf2, 0xfa, 0xfb, 0xf1, 0x97, 0x2c, 0xbf, 0xba, 0x9e, 0x6e, 0x3c, 0x15, 0x1e, 0x70, 0x45, 0xe3, 0x86, 0xb1, 0x6f, 0xe9, 0xea, 0x0a, 0x5e, 0x0e, 0x86, 0xb3, 0x2a, 0x3e, 0x5a, 
		0x1c, 0xe7, 0x1f, 0x77, 0xfa, 0x06, 0x3d, 0x4e, 0xb9, 0xdc, 0x65, 0x29, 0x0f, 0x1d, 0xe7, 0x99, 0xd6, 0x89, 0x3e, 0x80, 0x25, 0xc8, 0x66, 0x52, 0x78, 0xc9, 0x4c, 0x2e, 0x6a, 0xb3, 0x10, 0x9c, 
		0xba, 0x0e, 0x15, 0xc6, 0x78, 0xea, 0xe2, 0x94, 0x53, 0x3c, 0xfc, 0xa5, 0xf4, 0x2d, 0x0a, 0x1e, 0xa7, 0x4e, 0xf7, 0xf2, 0x3d, 0x2b, 0x1d, 0x36, 0x0f, 0x26, 0x39, 0x19, 0x60, 0x79, 0xc2, 0x19, 
		0x08, 0xa7, 0x23, 0x52, 0xb6, 0x12, 0x13, 0xf7, 0x6e, 0xfe, 0xad, 0xeb, 0x66, 0x1f, 0xc3, 0xea, 0x95, 0x45, 0xbc, 0xe3, 0x83, 0xc8, 0x7b, 0xa6, 0xd1, 0x37, 0x7f, 0xb1, 0x28, 0xff, 0x8c, 0x01, 
		0xef, 0xdd, 0x32, 0xc3, 0xa5, 0x5a, 0x6c, 0xbe, 0x85, 0x21, 0x58, 0x65, 0x02, 0x98, 0xab, 0x68, 0x0f, 0xa5, 0xce, 0xee, 0x3b, 0x95, 0x2f, 0xdb, 0xad, 0x7d, 0xef, 0x2a, 0x84, 0x2f, 0x6e, 0x5b, 
		0x28, 0xb6, 0x21, 0x15, 0x70, 0x61, 0x07, 0x29, 0x75, 0x47, 0xdd, 0xec, 0x10, 0x15, 0x9f, 0x61, 0x30, 0xa8, 0xcc, 0x13, 0x96, 0xbd, 0x61, 0xeb, 0x1e, 0xfe, 0x34, 0x03, 0xcf, 0x63, 0x03, 0xaa, 
		0x90, 0x5c, 0x73, 0xb5, 0x39, 0xa2, 0x70, 0x4c, 0x0b, 0x9e, 0x9e, 0xd5, 0x14, 0xde, 0xaa, 0xcb, 0xbc, 0x86, 0xcc, 0xee, 0xa7, 0x2c, 0x62, 0x60, 0xab, 0x5c, 0xab, 0x9c, 0x6e, 0x84, 0xf3, 0xb2, 
		0xaf, 0x1e, 0x8b, 0x64, 0xca, 0xf0, 0xbd, 0x19, 0xb9, 0x69, 0x23, 0xa0, 0x50, 0xbb, 0x5a, 0x65, 0x32, 0x5a, 0x68, 0x40, 0xb3, 0xb4, 0x2a, 0x3c, 0xd5, 0xe9, 0x9e, 0x31, 0xf7, 0xb8, 0x21, 0xc0, 
		0x19, 0x0b, 0x54, 0x9b, 0x99, 0xa0, 0x5f, 0x87, 0x7e, 0x99, 0xf7, 0x95, 0xa8, 0x7d, 0x3d, 0x62, 0x9a, 0x88, 0x37, 0xf8, 0x77, 0x2d, 0xe3, 0x97, 0x5f, 0x93, 0xed, 0x11, 0x81, 0x12, 0x68, 0x16, 
		0x29, 0x88, 0x35, 0x0e, 0xd6, 0x1f, 0xe6, 0xc7, 0xa1, 0xdf, 0xde, 0x96, 0x99, 0xba, 0x58, 0x78, 0xa5, 0x84, 0xf5, 0x57, 0x63, 0x72, 0x22, 0x1b, 0xff, 0xc3, 0x83, 0x9b, 0x96, 0x46, 0xc2, 0x1a, 
		0xeb, 0x0a, 0xb3, 0xcd, 0x54, 0x30, 0x2e, 0x53, 0xe4, 0x48, 0xd9, 0x8f, 0x28, 0x31, 0xbc, 0x6d, 0xef, 0xf2, 0xeb, 0x58, 0xea, 0xff, 0xc6, 0x34, 0x61, 0xed, 0x28, 0xfe, 0x73, 0x3c, 0x7c, 0xee, 
		0xd9, 0x14, 0x4a, 0x5d, 0xe3, 0xb7, 0x64, 0xe8, 0x14, 0x5d, 0x10, 0x42, 0xe0, 0x13, 0x3e, 0x20, 0xb6, 0xe2, 0xee, 0x45, 0xea, 0xab, 0xaa, 0xa3, 0x15, 0x4f, 0x6c, 0xdb, 0xd0, 0x4f, 0xcb, 0xfa, 
		0x42, 0xf4, 0x42, 0xc7, 0xb5, 0xbb, 0x6a, 0xef, 0x1d, 0x3b, 0x4f, 0x65, 0x05, 0x21, 0xcd, 0x41, 0x9e, 0x79, 0x1e, 0xd8, 0xc7, 0x4d, 0x85, 0x86, 0x6a, 0x47, 0x4b, 0xe4, 0x50, 0x62, 0x81, 0x3d, 
		0xf2, 0xa1, 0x62, 0xcf, 0x46, 0x26, 0x8d, 0x5b, 0xa0, 0x83, 0x88, 0xfc, 0xa3, 0xb6, 0xc7, 0xc1, 0xc3, 0x24, 0x15, 0x7f, 0x92, 0x74, 0xcb, 0x69, 0x0b, 0x8a, 0x84, 0x47, 0x85, 0xb2, 0x92, 0x56, 
		0x00, 0xbf, 0x5b, 0x09, 0x9d, 0x48, 0x19, 0xad, 0x74, 0xb1, 0x62, 0x14, 0x00, 0x0e, 0x82, 0x23, 0x2a, 0x8d, 0x42, 0x58, 0xea, 0xf5, 0x55, 0x0c, 0x3e, 0xf4, 0xad, 0x1d, 0x61, 0x70, 0x3f, 0x23, 
		0x92, 0xf0, 0x72, 0x33, 0x41, 0x7e, 0x93, 0x8d, 0xf1, 0xec, 0x5f, 0xd6, 0xdb, 0x3b, 0x22, 0x6c, 0x59, 0x37, 0xde, 0x7c, 0x60, 0x74, 0xee, 0xcb, 0xa7, 0xf2, 0x85, 0x40, 0x6e, 0x32, 0x77, 0xce, 
		0x84, 0x80, 0x07, 0xa6, 0x9e, 0x50, 0xf8, 0x19, 0x55, 0xd8, 0xef, 0xe8, 0x35, 0x97, 0xd9, 0x61, 0xaa, 0xa7, 0x69, 0xa9, 0xc2, 0x06, 0x0c, 0xc5, 0xfc, 0xab, 0x04, 0x5a, 0xdc, 0xca, 0x0b, 0x80, 
		0x2e, 0x7a, 0x44, 0x9e, 0x84, 0x34, 0x45, 0xc3, 0x05, 0x67, 0xd5, 0xfd, 0xc9, 0x9e, 0x1e, 0x0e, 0xd3, 0xdb, 0x73, 0xdb, 0xcd, 0x88, 0x55, 0x10, 0x79, 0xda, 0x5f, 0x67, 0x40, 0x43, 0x67, 0xe3, 
		0x65, 0x34, 0xc4, 0xc5, 0xd8, 0x38, 0x3e, 0x71, 0x9e, 0xf8, 0x28, 0x3d, 0x20, 0xff, 0x6d, 0xf1, 0xe7, 0x21, 0x3e, 0x15, 0x4a, 0x3d, 0xb0, 0x8f, 0x2b, 0x9f, 0xe3, 0xe6, 0xf7, 0xad, 0x83, 0xdb, 
		0x68, 0x5a, 0x3d, 0xe9, 0xf7, 0x40, 0x81, 0x94, 0x1c, 0x26, 0x4c, 0xf6, 0x34, 0x29, 0x69, 0x94, 0xf7, 0x20, 0x15, 0x41, 0xf7, 0xd4, 0x02, 0x76, 0x2e, 0x6b, 0xf4, 0xbc, 0x68, 0x00, 0xa2, 0xd4, 
		0x71, 0x24, 0x08, 0xd4, 0x6a, 0xf4, 0x20, 0x33, 0xb7, 0xd4, 0xb7, 0x43, 0xaf, 0x61, 0x00, 0x50, 0x2e, 0xf6, 0x39, 0x1e, 0x46, 0x45, 0x24, 0x97, 0x74, 0x4f, 0x21, 0x14, 0x40, 0x88, 0x8b, 0xbf, 
		0x1d, 0xfc, 0x95, 0x4d, 0xaf, 0x91, 0xb5, 0x96, 0xd3, 0xdd, 0xf4, 0x70, 0x45, 0x2f, 0xa0, 0x66, 0xec, 0x09, 0xbc, 0xbf, 0x85, 0x97, 0xbd, 0x03, 0xd0, 0x6d, 0xac, 0x7f, 0x04, 0x85, 0xcb, 0x31, 
		0xb3, 0x27, 0xeb, 0x96, 0x41, 0x39, 0xfd, 0x55, 0xe6, 0x47, 0x25, 0xda, 0x9a, 0x0a, 0xca, 0xab, 0x25, 0x78, 0x50, 0x28, 0xf4, 0x29, 0x04, 0x53, 0xda, 0x86, 0x2c, 0x0a, 0xfb, 0x6d, 0xb6, 0xe9, 
		0x62, 0x14, 0xdc, 0x68, 0x00, 0x69, 0x48, 0xd7, 0xa4, 0xc0, 0x0e, 0x68, 0xee, 0x8d, 0xa1, 0x27, 0xa2, 0xfe, 0x3f, 0x4f, 0x8c, 0xad, 0x87, 0xe8, 0x06, 0xe0, 0x8c, 0xb5, 0xb6, 0xd6, 0xf4, 0x7a, 
		0x7c, 0x1e, 0xce, 0xaa, 0xec, 0x5f, 0x37, 0xd3, 0x99, 0xa3, 0x78, 0xce, 0x42, 0x2a, 0x6b, 0x40, 0x35, 0x9e, 0xfe, 0x20, 0xb9, 0x85, 0xf3, 0xd9, 0xab, 0xd7, 0x39, 0xee, 0x8b, 0x4e, 0x12, 0x3b, 
		0xf7, 0xfa, 0xc9, 0x1d, 0x56, 0x18, 0x6d, 0x4b, 0x31, 0x66, 0xa3, 0x26, 0xb2, 0x97, 0xe3, 0xea, 0x74, 0xfa, 0x6e, 0x3a, 0x32, 0x43, 0x5b, 0xdd, 0xf7, 0xe7, 0x41, 0x68, 0xfb, 0x20, 0x78, 0xca, 
		0x4e, 0xf5, 0x0a, 0xfb, 0x97, 0xb3, 0xfe, 0xd8, 0xac, 0x56, 0x40, 0x45, 0x27, 0x95, 0x48, 0xba, 0x3a, 0x3a, 0x53, 0x55, 0x87, 0x8d, 0x83, 0x20, 0xb7, 0xa9, 0x6b, 0xfe, 0x4b, 0x95, 0x96, 0xd0, 
		0xbc, 0x67, 0xa8, 0x55, 0x58, 0x9a, 0x15, 0xa1, 0x63, 0x29, 0xa9, 0xcc, 0x33, 0xdb, 0xe1, 0x99, 0x56, 0x4a, 0x2a, 0xa6, 0xf9, 0x25, 0x31, 0x3f, 0x1c, 0x7e, 0xf4, 0x5e, 0x7c, 0x31, 0x29, 0x90, 
		0x02, 0xe8, 0xf8, 0xfd, 0x70, 0x2f, 0x27, 0x04, 0x5c, 0x15, 0xbb, 0x80, 0xe3, 0x2c, 0x28, 0x05, 0x48, 0x15, 0xc1, 0x95, 0x22, 0x6d, 0xc6, 0xe4, 0x3f, 0x13, 0xc1, 0x48, 0xdc, 0x86, 0x0f, 0xc7, 
		0xee, 0xc9, 0xf9, 0x07, 0x0f, 0x1f, 0x04, 0x41, 0xa4, 0x79, 0x47, 0x40, 0x17, 0x6e, 0x88, 0x5d, 0xeb, 0x51, 0x5f, 0x32, 0xd1, 0xc0, 0x9b, 0xd5, 0x8f, 0xc1, 0xbc, 0xf2, 0x64, 0x35, 0x11, 0x41, 
		0x34, 0x78, 0x7b, 0x25, 0x60, 0x9c, 0x2a, 0x60, 0xa3, 0xe8, 0xf8, 0xdf, 0x1b, 0x6c, 0x63, 0x1f, 0xc2, 0xb4, 0x12, 0x0e, 0x9e, 0x32, 0xe1, 0x02, 0xd1, 0x4f, 0x66, 0xaf, 0x15, 0x81, 0xd1, 0xca, 
		0xe0, 0x95, 0x23, 0x6b, 0xe1, 0x92, 0x3e, 0x33, 0x62, 0x0b, 0x24, 0x3b, 0x22, 0xb9, 0xbe, 0xee, 0x0e, 0xa2, 0xb2, 0x85, 0x99, 0x0d, 0xba, 0xe6, 0x8c, 0x0c, 0x72, 0xde, 0x28, 0xf7, 0xa2, 0x2d, 
		0x45, 0x78, 0x12, 0xd0, 0xfd, 0x94, 0xb7, 0x95, 0x62, 0x08, 0x7d, 0x64, 0xf0, 0xf5, 0xcc, 0xe7, 0x6f, 0xa3, 0x49, 0x54, 0xfa, 0x48, 0x7d, 0x87, 0x27, 0xfd, 0x9d, 0xc3, 0x1e, 0x8d, 0x3e, 0xf3, 
		0x41, 0x63, 0x47, 0x0a, 0x74, 0xff, 0x2e, 0x99, 0xab, 0x6e, 0x6f, 0x3a, 0x37, 0xfd, 0xf8, 0xf4, 0x60, 0xdc, 0x12, 0xa8, 0xf8, 0xdd, 0xeb, 0xa1, 0x4c, 0xe1, 0x1b, 0x99, 0x0d, 0x6b, 0x6e, 0xdb, 
		0x10, 0x55, 0x7b, 0xc6, 0x37, 0x2c, 0x67, 0x6d, 0x3b, 0xd4, 0x65, 0x27, 0x04, 0xe8, 0xd0, 0xdc, 0xc7, 0x0d, 0x29, 0xf1, 0xa3, 0xff, 0x00, 0xcc, 0x92, 0x0f, 0x39, 0xb5, 0x0b, 0xed, 0x0f, 0x69, 
		0xfb, 0x9f, 0x7b, 0x66, 0x9c, 0x7d, 0xdb, 0xce, 0x0b, 0xcf, 0x91, 0xa0, 0xa3, 0x5e, 0x15, 0xd9, 0x88, 0x2f, 0x13, 0xbb, 0x24, 0xad, 0x5b, 0x51, 0xbf, 0x79, 0x94, 0x7b, 0xeb, 0xd6, 0x3b, 0x76, 
		0xb3, 0x2e, 0x39, 0x37, 0x79, 0x59, 0x11, 0xcc, 0x97, 0xe2, 0x26, 0x80, 0x2d, 0x31, 0x2e, 0xf4, 0xa7, 0xad, 0x42, 0x68, 0x3b, 0x2b, 0x6a, 0xc6, 0xcc, 0x4c, 0x75, 0x12, 0x1c, 0xf1, 0x2e, 0x78, 
		0x37, 0x42, 0x12, 0x6a, 0xe7, 0x51, 0x92, 0xb7, 0xe6, 0xbb, 0xa1, 0x06, 0x50, 0x63, 0xfb, 0x4b, 0x18, 0x10, 0x6b, 0x1a, 0xfa, 0xed, 0xca, 0x11, 0xd8, 0xbd, 0x25, 0x3d, 0xc9, 0xc3, 0xe1, 0xe2, 
		0x59, 0x16, 0x42, 0x44, 0x86, 0x13, 0x12, 0x0a, 0x6e, 0xec, 0x0c, 0xd9, 0x2a, 0xea, 0xab, 0xd5, 0x4e, 0x67, 0xaf, 0x64, 0x5f, 0xa8, 0x86, 0xda, 0x88, 0xe9, 0xbf, 0xbe, 0xfe, 0xc3, 0xe4, 0x64, 
		0x57, 0x80, 0xbc, 0x9d, 0x86, 0xc0, 0xf7, 0xf0, 0xf8, 0x7b, 0x78, 0x60, 0x4d, 0x60, 0x03, 0x60, 0x46, 0x83, 0xfd, 0xd1, 0xb0, 0x1f, 0x38, 0xf6, 0x04, 0xae, 0x45, 0x77, 0xcc, 0xfc, 0x36, 0xd7, 
		0x33, 0x6b, 0x42, 0x83, 0x71, 0xab, 0x1e, 0xf0, 0x87, 0x41, 0x80, 0xb0, 0x5f, 0x5e, 0x00, 0x3c, 0xbe, 0x57, 0xa0, 0x77, 0x24, 0xae, 0xe8, 0xbd, 0x99, 0x42, 0x46, 0x55, 0x61, 0x2e, 0x58, 0xbf, 
		0x8f, 0xf4, 0x58, 0x4e, 0xa2, 0xfd, 0xdd, 0xf2, 0x38, 0xef, 0x74, 0xf4, 0xc2, 0xbd, 0x89, 0x87, 0xc3, 0xf9, 0x66, 0x53, 0x74, 0x8e, 0xb3, 0xc8, 0x55, 0xf2, 0x75, 0xb4, 0xb9, 0xd9, 0xfc, 0x46, 
		0x61, 0x26, 0xeb, 0x7a, 0x84, 0xdf, 0x1d, 0x8b, 0x79, 0x0e, 0x6a, 0x84, 0xe2, 0x95, 0x5f, 0x91, 0x8e, 0x59, 0x6e, 0x46, 0x70, 0x57, 0xb4, 0x20, 0x91, 0x55, 0xd5, 0x8c, 0x4c, 0xde, 0x02, 0xc9, 
		0xe1, 0xac, 0x0b, 0xb9, 0xd0, 0x05, 0x82, 0xbb, 0x48, 0x62, 0xa8, 0x11, 0x9e, 0xa9, 0x74, 0x75, 0xb6, 0x19, 0x7f, 0xb7, 0x09, 0xdc, 0xa9, 0xe0, 0xa1, 0x09, 0x2d, 0x66, 0x33, 0x46, 0x32, 0xc4, 
		0x02, 0x1f, 0x5a, 0xe8, 0x8c, 0xbe, 0xf0, 0x09, 0x25, 0xa0, 0x99, 0x4a, 0x10, 0xfe, 0x6e, 0x1d, 0x1d, 0x3d, 0xb9, 0x1a, 0xdf, 0xa4, 0xa5, 0x0b, 0x0f, 0xf2, 0x86, 0xa1, 0x69, 0xf1, 0x68, 0x28, 
		0x83, 0xda, 0xb7, 0xdc, 0xfe, 0x06, 0x39, 0x57, 0x9b, 0xce, 0xe2, 0xa1, 0x52, 0x7f, 0xcd, 0x4f, 0x01, 0x5e, 0x11, 0x50, 0xfa, 0x83, 0x06, 0xa7, 0xc4, 0xb5, 0x02, 0xa0, 0x27, 0xd0, 0xe6, 0x0d, 
		0x27, 0x8c, 0xf8, 0x9a, 0x41, 0x86, 0x3f, 0x77, 0x06, 0x4c, 0x60, 0xc3, 0xb5, 0x06, 0xa8, 0x61, 0x28, 0x7a, 0x17, 0xf0, 0xe0, 0x86, 0xf5, 0xc0, 0xaa, 0x58, 0x60, 0x00, 0x62, 0x7d, 0xdc, 0x30, 
		0xd7, 0x9e, 0xe6, 0x11, 0x63, 0xea, 0x38, 0x23, 0x94, 0xdd, 0xc2, 0x53, 0x34, 0x16, 0xc2, 0xc2, 0x56, 0xee, 0xcb, 0xbb, 0xde, 0xb6, 0xbc, 0x90, 0xa1, 0x7d, 0xfc, 0xeb, 0x76, 0x1d, 0x59, 0xce, 
		0x09, 0xe4, 0x05, 0x6f, 0x88, 0x01, 0x7c, 0x4b, 0x3d, 0x0a, 0x72, 0x39, 0x24, 0x7c, 0x92, 0x7c, 0x5f, 0x72, 0xe3, 0x86, 0xb9, 0x9d, 0x4d, 0x72, 0xb4, 0x5b, 0xc1, 0x1a, 0xfc, 0xb8, 0x9e, 0xd3, 
		0x78, 0x55, 0x54, 0xed, 0xb5, 0xa5, 0xfc, 0x08, 0xd3, 0x7c, 0x3d, 0xd8, 0xc4, 0x0f, 0xad, 0x4d, 0x5e, 0xef, 0x50, 0x1e, 0xf8, 0xe6, 0x61, 0xb1, 0xd9, 0x14, 0x85, 0xa2, 0x3c, 0x13, 0x51, 0x6c, 
		0xe7, 0xc7, 0xd5, 0x6f, 0xc4, 0x4e, 0xe1, 0x56, 0xce, 0xbf, 0x2a, 0x36, 0x37, 0xc8, 0xc6, 0xdd, 0x34, 0x32, 0x9a, 0xd7, 0x12, 0x82, 0x63, 0x92, 0x8e, 0xfa, 0x0e, 0x67, 0xe0, 0x00, 0x60, 0x40, 
		0x37, 0xce, 0x39, 0x3a, 0xcf, 0xf5, 0xfa, 0xd3, 0x37, 0x77, 0xc2, 0xab, 0x1b, 0x2d, 0xc5, 0x5a, 0x9e, 0x67, 0xb0, 0x5c, 0x42, 0x37, 0xa3, 0x4f, 0x40, 0x27, 0x82, 0xd3, 0xbe, 0x9b, 0xbc, 0x99, 
		0x9d, 0x8e, 0x11, 0xd5, 0x15, 0x73, 0x0f, 0xbf, 0x7e, 0x1c, 0x2d, 0xd6, 0x7b, 0xc4, 0x00, 0xc7, 0x6b, 0x1b, 0x8c, 0xb7, 0x45, 0x90, 0xa1, 0x21, 0xbe, 0xb1, 0x6e, 0xb2, 0xb4, 0x6e, 0x36, 0x6a, 
		0x2f, 0xab, 0x48, 0x57, 0x79, 0x6e, 0x94, 0xbc, 0xd2, 0x76, 0xa3, 0xc6, 0xc8, 0xc2, 0x49, 0x65, 0xee, 0xf8, 0x0f, 0x53, 0x7d, 0xde, 0x8d, 0x46, 0x1d, 0x0a, 0x73, 0xd5, 0xc6, 0x4d, 0xd0, 0x4c, 
		0xdb, 0xbb, 0x39, 0x29, 0x50, 0x46, 0xba, 0xa9, 0xe8, 0x26, 0x95, 0xac, 0x04, 0xe3, 0x5e, 0xbe, 0xf0, 0xd5, 0xfa, 0xa1, 0x9a, 0x51, 0x2d, 0x6a, 0xe2, 0x8c, 0xef, 0x63, 0x22, 0xee, 0x86, 0x9a, 
		0xb8, 0xc2, 0x89, 0xc0, 0xf6, 0x2e, 0x24, 0x43, 0xaa, 0x03, 0x1e, 0xa5, 0xa4, 0xd0, 0xf2, 0x9c, 0xba, 0x61, 0xc0, 0x83, 0x4d, 0x6a, 0xe9, 0x9b, 0x50, 0x15, 0xe5, 0x8f, 0xd6, 0x5b, 0x64, 0xba, 
		0xf9, 0xa2, 0x26, 0x28, 0xe1, 0x3a, 0x3a, 0xa7, 0x86, 0x95, 0xa9, 0x4b, 0xe9, 0x62, 0x55, 0xef, 0xd3, 0xef, 0x2f, 0xc7, 0xda, 0xf7, 0x52, 0xf7, 0x69, 0x6f, 0x04, 0x3f, 0x59, 0x0a, 0xfa, 0x77, 
		0x15, 0xa9, 0xe4, 0x80, 0x01, 0x86, 0xb0, 0x87, 0xad, 0xe6, 0x09, 0x9b, 0x93, 0xe5, 0x3e, 0x3b, 0x5a, 0xfd, 0x90, 0xe9, 0x97, 0xd7, 0x34, 0x9e, 0xd9, 0xb7, 0xf0, 0x2c, 0x51, 0x8b, 0x2b, 0x02, 
		0x3a, 0xac, 0xd5, 0x96, 0x7d, 0xa6, 0x7d, 0x01, 0xd6, 0x3e, 0xcf, 0xd1, 0x28, 0x2d, 0x7d, 0x7c, 0xcf, 0x25, 0x9f, 0x1f, 0x9b, 0xb8, 0xf2, 0xad, 0x72, 0xb4, 0xd6, 0x5a, 0x4c, 0xf5, 0x88, 0x5a, 
		0x71, 0xac, 0x29, 0xe0, 0xe6, 0xa5, 0x19, 0xe0, 0xfd, 0xac, 0xb0, 0x47, 0x9b, 0xfa, 0x93, 0xed, 0x8d, 0xc4, 0xd3, 0xe8, 0xcc, 0x57, 0x3b, 0x28, 0x29, 0x66, 0xd5, 0xf8, 0x28, 0x2e, 0x13, 0x79,
		0x91, 0x01, 0x5f, 0x78, 0x55, 0x60, 0x75, 0xed, 0x44, 0x0e, 0x96, 0xf7, 0x8c, 0x5e, 0xd3, 0xe3, 0xd4, 0x6d, 0x05, 0x15, 0xba, 0x6d, 0xf4, 0x88, 0x25, 0x61, 0xa1, 0x03, 0xbd, 0xf0, 0x64, 0x05, 
		0x15, 0x9e, 0xeb, 0xc3, 0xa2, 0x57, 0x90, 0x3c, 0xec, 0x1a, 0x27, 0x97, 0x2a, 0x07, 0x3a, 0xa9, 0x9b, 0x6d, 0x3f, 0x1b, 0xf5, 0x21, 0x63, 0x1e, 0xfb, 0x66, 0x9c, 0xf5, 0x19, 0xf3, 0xdc, 0x26, 
		0x28, 0xd9, 0x33, 0x75, 0xf5, 0xfd, 0x55, 0xb1, 0x82, 0x34, 0x56, 0x03, 0xbb, 0x3c, 0xba, 0x8a, 0x11, 0x77, 0x51, 0x28, 0xf8, 0xd9, 0x0a, 0xc2, 0x67, 0x51, 0xcc, 0xab, 0x5f, 0x92, 0xad, 0xcc, 
		0x51, 0x17, 0xe8, 0x4d, 0x8e, 0xdc, 0x30, 0x38, 0x62, 0x58, 0x9d, 0x37, 0x91, 0xf9, 0x20, 0x93, 0xc2, 0x90, 0x7a, 0xea, 0xce, 0x7b, 0x3e, 0xfb, 0x64, 0xce, 0x21, 0x51, 0x32, 0xbe, 0x4f, 0x77, 
		0x7e, 0xe3, 0xb6, 0xa8, 0x46, 0x3d, 0x29, 0xc3, 0x69, 0x53, 0xde, 0x48, 0x80, 0xe6, 0x13, 0x64, 0x10, 0x08, 0xae, 0xa2, 0x24, 0xb2, 0x6d, 0xdd, 0xfd, 0x2d, 0x85, 0x69, 0x66, 0x21, 0x07, 0x09, 
		0x0a, 0x46, 0x9a, 0xb3, 0xdd, 0xc0, 0x45, 0x64, 0xcf, 0xde, 0x6c, 0x58, 0xae, 0xc8, 0x20, 0x1c, 0xdd, 0xf7, 0xbe, 0x5b, 0x40, 0x8d, 0x58, 0x1b, 0x7f, 0x01, 0xd2, 0xcc, 0xbb, 0xe3, 0xb4, 0x6b, 
		0x7e, 0x6a, 0xa2, 0xdd, 0x45, 0xff, 0x59, 0x3a, 0x44, 0x0a, 0x35, 0x3e, 0xd5, 0xcd, 0xb4, 0xbc, 0xa8, 0xce, 0xea, 0x72, 0xbb, 0x84, 0x64, 0xfa, 0xae, 0x12, 0x66, 0x8d, 0x47, 0x6f, 0x3c, 0xbf, 
		0x63, 0xe4, 0x9b, 0xd2, 0x9e, 0x5d, 0x2f, 0x54, 0x1b, 0x77, 0xc2, 0xae, 0x70, 0x63, 0x4e, 0xf6, 0x8d, 0x0d, 0x0e, 0x74, 0x57, 0x13, 0x5b, 0xe7, 0x71, 0x16, 0x72, 0xf8, 0x5d, 0x7d, 0x53, 0xaf, 
		0x08, 0xcb, 0x40, 0x40, 0xcc, 0xe2, 0xb4, 0x4e, 0x6a, 0x46, 0xd2, 0x34, 0x84, 0xaf, 0x15, 0x01, 0x28, 0x04, 0xb0, 0xe1, 0x1d, 0x3a, 0x98, 0x95, 0xb4, 0x9f, 0xb8, 0x06, 0x48, 0xa0, 0x6e, 0xce, 
		0x82, 0x3b, 0x3f, 0x6f, 0x82, 0xab, 0x20, 0x35, 0x4b, 0x1d, 0x1a, 0x01, 0xf8, 0x27, 0x72, 0x27, 0xb1, 0x60, 0x15, 0x61, 0xdc, 0x3f, 0x93, 0xe7, 0x2b, 0x79, 0x3a, 0xbb, 0xbd, 0x25, 0x45, 0x34, 
		0xe1, 0x39, 0x88, 0xa0, 0x4b, 0x79, 0xce, 0x51, 0xb7, 0xc9, 0x32, 0x2f, 0xc9, 0xba, 0x1f, 0xa0, 0x7e, 0xc8, 0x1c, 0xe0, 0xf6, 0xd1, 0xc7, 0xbc, 0xc3, 0x11, 0x01, 0xcf, 0xc7, 0xaa, 0xe8, 0xa1, 
		0x49, 0x87, 0x90, 0x1a, 0x9a, 0xbd, 0x4f, 0xd4, 0xcb, 0xde, 0xda, 0xd0, 0x38, 0xda, 0x0a, 0xd5, 0x2a, 0xc3, 0x39, 0x03, 0x67, 0x36, 0x91, 0xc6, 0x7c, 0x31, 0xf9, 0x8d, 0x4f, 0x2b, 0xb1, 0xe0, 
		0xb7, 0x59, 0x9e, 0xf7, 0x3a, 0xbb, 0xf5, 0x43, 0xff, 0x19, 0xd5, 0xf2, 0x9c, 0x45, 0xd9, 0x27, 0x2c, 0x22, 0x97, 0xbf, 0x2a, 0xfc, 0xe6, 0x15, 0x71, 0xfc, 0x91, 0x0f, 0x25, 0x15, 0x94, 0x9b, 
		0x61, 0x93, 0xe5, 0xfa, 0xeb, 0x9c, 0xb6, 0xce, 0x59, 0x64, 0xa8, 0xc2, 0xd1, 0xa8, 0xba, 0x12, 0x5e, 0x07, 0xc1, 0xb6, 0x0c, 0x6a, 0x05, 0xe3, 0x65, 0x50, 0xd2, 0x10, 0x42, 0xa4, 0x03, 0xcb, 
		0x0e, 0x6e, 0xec, 0xe0, 0x3b, 0xdb, 0x98, 0x16, 0xbe, 0xa0, 0x98, 0x4c, 0x64, 0xe9, 0x78, 0x32, 0x32, 0x95, 0x1f, 0x9f, 0xdf, 0x92, 0xd3, 0xe0, 0x2b, 0x34, 0xa0, 0xd3, 0x1e, 0xf2, 0x71, 0x89, 
		0x41, 0x74, 0x0a, 0x1b, 0x8c, 0x34, 0xa3, 0x4b, 0x20, 0x71, 0xbe, 0xc5, 0xd8, 0x32, 0x76, 0xc3, 0x8d, 0x9f, 0x35, 0xdf, 0x2e, 0x2f, 0x99, 0x9b, 0x47, 0x6f, 0x0b, 0xe6, 0x1d, 0xf1, 0xe3, 0x0f, 
		0x54, 0xda, 0x4c, 0xe5, 0x91, 0xd8, 0xda, 0x1e, 0xcf, 0x79, 0x62, 0xce, 0x6f, 0x7e, 0x3e, 0xcd, 0x66, 0xb1, 0x18, 0x16, 0x05, 0x1d, 0x2c, 0xfd, 0xc5, 0xd2, 0x8f, 0x84, 0x99, 0x22, 0xfb, 0xf6, 
		0x57, 0xf3, 0x23, 0xf5, 0x23, 0x76, 0x32, 0xa6, 0x31, 0x35, 0xa8, 0x93, 0x02, 0xcd, 0xcc, 0x56, 0x62, 0x81, 0xf0, 0xac, 0xb5, 0xeb, 0x75, 0x5a, 0x97, 0x36, 0x16, 0x6e, 0xcc, 0x73, 0xd2, 0x88, 
		0x92, 0x62, 0x96, 0xde, 0xd0, 0x49, 0xb9, 0x81, 0x1b, 0x90, 0x50, 0x4c, 0x14, 0x56, 0xc6, 0x71, 0xbd, 0xc7, 0xc6, 0xe6, 0x0a, 0x14, 0x7a, 0x32, 0x06, 0xd0, 0xe1, 0x45, 0x9a, 0x7b, 0xf2, 0xc3, 
		0xfd, 0x53, 0xaa, 0xc9, 0x00, 0x0f, 0xa8, 0x62, 0xe2, 0xbf, 0x25, 0xbb, 0xf6, 0xd2, 0xbd, 0x35, 0x05, 0x69, 0x12, 0x71, 0x22, 0x02, 0x04, 0xb2, 0x7c, 0xcf, 0xcb, 0xb6, 0x2b, 0x9c, 0x76, 0xcd, 
		0xc0, 0x3e, 0x11, 0x53, 0xd3, 0xe3, 0x40, 0x16, 0x60, 0xbd, 0xab, 0x38, 0xf0, 0xad, 0x47, 0x25, 0x9c, 0x20, 0x38, 0xba, 0x76, 0xce, 0x46, 0xf7, 0xc5, 0xa1, 0xaf, 0x77, 0x60, 0x60, 0x75, 0x20, 
		0x4e, 0xfe, 0xcb, 0x85, 0xd8, 0x8d, 0xe8, 0x8a, 0xb0, 0xf9, 0xaa, 0x7a, 0x7e, 0xaa, 0xf9, 0x4c, 0x5c, 0xc2, 0x48, 0x19, 0x8c, 0x8a, 0xfb, 0x02, 0xe4, 0x6a, 0xc3, 0x01, 0xf9, 0xe1, 0xeb, 0xd6, 
		0x69, 0xf8, 0xd4, 0x90, 0xa0, 0xde, 0x5c, 0xa6, 0x2d, 0x25, 0x09, 0x3f, 0x9f, 0xe6, 0x08, 0xc2, 0x32, 0x61, 0x4e, 0xb7, 0x5b, 0xe2, 0x77, 0xce, 0xe3, 0xdf, 0x8f, 0x57, 0xe6, 0x72, 0xc3, 0x3a
};

uint32_t F(_BLOWFISH_CTX* _ctx, uint32_t x) {
	unsigned short a;
	unsigned short b;
	unsigned short c;
	unsigned short d;
	uint32_t  	   y;

	d = x & 0x00FF;
	x >>= 8;
	c = x & 0x00FF;
	x >>= 8;
	b = x & 0x00FF;
	x >>= 8;
	a = x & 0x00FF;
	//y = ((_ctx->S[0][a] + _ctx->S[1][b]) ^ _ctx->S[2][c]) + _ctx->S[3][d];
	y = _ctx->S[0][a] + _ctx->S[1][b];
	y = y ^ _ctx->S[2][c];
	y = y + _ctx->S[3][d];

	return y;
}

void Blowfish_Init(_BLOWFISH_CTX* _ctx, char _key[], int _keybytes) {

	short          i;
	short          j;
	short          k;
	uint32_t  	   data;
	uint32_t  	   datal;
	uint32_t  	   datar;

#if defined(_WIN32)
	memcpy_s(_ctx->S, sizeof(_ctx->S), gBoxS, sizeof(gBoxS));
#elif defined(__linux__)
	memcpy(_ctx->S, gBoxS, sizeof(gBoxS));
#endif

	j = 0;
	for (i = 0; i < N + 2; ++i) {
		data = 0x00000000;
		for (k = 0; k < 4; ++k) {
			data = (data << 8) | _key[j];
			j = j + 1;
			if (j >= _keybytes) {
				j = 0;
			}
		}
		_ctx->P[i] = ((uint32_t*)gP)[i] ^ data;
	}

	datal = 0x00000000;
	datar = 0x00000000;

	for (i = 0; i < N + 2; i += 2) {
		Blowfish_Encrypt(_ctx, &datal, &datar);

		_ctx->P[i] = datal;
		_ctx->P[i + 1] = datar;
	}

	for (i = 0; i < 4; ++i) {
		for (j = 0; j < 256; j += 2) {

			Blowfish_Encrypt(_ctx, &datal, &datar);

			_ctx->S[i][j] = datal;
			_ctx->S[i][j + 1] = datar;
		}
	}
};

void Blowfish_Encrypt(_BLOWFISH_CTX* _ctx, uint32_t *_xl, uint32_t *_xr) {

	uint32_t  		Xl;
	uint32_t  		Xr;
	uint32_t  		temp;
	short          	i;

	Xl = *_xl;
	Xr = *_xr;

	for (i = 0; i < N; ++i) {
		Xl = Xl ^ _ctx->P[i];
		Xr = F(_ctx, Xl) ^ Xr;

		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}

	temp = Xl;
	Xl = Xr;
	Xr = temp;

	Xr = Xr ^ _ctx->P[N];
	Xl = Xl ^ _ctx->P[N + 1];

	*_xl = Xl;
	*_xr = Xr;
};

void Blowfish_Decrypt(_BLOWFISH_CTX* _ctx, uint32_t *_xl, uint32_t *_xr) {

	uint32_t  		Xl;
	uint32_t  		Xr;
	uint32_t  		temp;
	short           i;

	Xl = *_xl;
	Xr = *_xr;

	for (i = N + 1; i > 1; --i) {
		Xl = Xl ^ _ctx->P[i];
		Xr = F(_ctx, Xl) ^ Xr;

		/* Exchange Xl and Xr */
		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}

	/* Exchange Xl and Xr */
	temp = Xl;
	Xl = Xr;
	Xr = temp;

	Xr = Xr ^ _ctx->P[1];
	Xl = Xl ^ _ctx->P[0];

	*_xl = Xl;
	*_xr = Xr;
};

