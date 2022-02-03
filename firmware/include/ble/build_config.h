/**
 * @file build_config.h
 * @brief Header file for API functions for the sensor interface.
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#ifndef INCLUDE_BUILD_CONFIG_H_
#define INCLUDE_BUILD_CONFIG_H_

#ifdef CFG_FULL_BUILD_CONFIG

#define  MONTANA_CID					101
#define  CFG_SLEEP
#define  CFG_BLE
#define  CFG_WLAN_COEX
#define  CFG_SEC_CON					32
#define  CFG_ALLROLES
#define  CFG_ATTS
#define  CFG_ATTC
#define  CFG_CON						10
#define  CFG_ACT 						11
#define  CFG_EMB
#define  CFG_HOST
#define  CFG_RF_ATLAS
#define  CFG_RAL						3
#define  CFG_ALLPRF
#define  CFG_PRF
#define  CFG_NB_PRF						39
#define  CFG_EXT_DB
#define  CFG_CON_CTE_REQ
#define  CFG_CON_CTE_RSP
#define  CFG_CONLESS_CTE_TX
#define  CFG_CONLESS_CTE_RX
#define  CFG_AOA
#define  CFG_AOD
#define  CFG_APP
#define  CFG_PRF_ANPC
#define  CFG_PRF_ANPS
#define  CFG_PRF_BASC
#define  CFG_PRF_BASS
#define  CFG_PRF_BLPC
#define  CFG_PRF_BLPS
#define  CFG_PRF_CGMC
#define  CFG_PRF_CGMS
#define  CFG_PRF_CPPC
#define  CFG_PRF_CPPS
#define  CFG_PRF_CSCPC
#define  CFG_PRF_CSCPS
#define  CFG_PRF_DISC
#define  CFG_PRF_DISS
#define  CFG_PRF_FMPT
#define  CFG_PRF_FMPL
#define  CFG_PRF_GLPC
#define  CFG_PRF_GLPS
#define  CFG_PRF_HOGPBH
#define  CFG_PRF_HOGPD
#define  CFG_PRF_HOGPRH
#define  CFG_PRF_HRPC
#define  CFG_PRF_HRPS
#define  CFG_PRF_HTPC
#define  CFG_PRF_HTPT
#define  CFG_PRF_LANC
#define  CFG_PRF_LANS
#define  CFG_PRF_PASPC
#define  CFG_PRF_PASPS
#define  CFG_PRF_PXPM
#define  CFG_PRF_PXPR
#define  CFG_PRF_RSCPC
#define  CFG_PRF_RSCPS
#define  CFG_PRF_SCPPC
#define  CFG_PRF_SCPPS
#define  CFG_PRF_TIPC
#define  CFG_PRF_TIPS
#define  CFG_PRF_WPTS
#define  CFG_PRF_WPTC
#define  CFG_PRF_GLPS
#define  CFG_PRF_GLPC

#endif

#ifdef  CFG_HCI_BUILD_CONFIG

#define  MONTANA_CID					101
#define  CFG_DBG
#define  CFG_DBG_MEM
#define  CFG_BLE
#define  CFG_WLAN_COEX
#define  CFG_SEC_CON					32
#define  CFG_ALLROLES
#define  CFG_ATTS
#define  CFG_ATTC
#define  CFG_CON						10
#define  CFG_ACT 						11
#define  CFG_EMB
#define  CFG_HOST
#define  CFG_RF_ATLAS
#define  CFG_RAL						3
#define  CFG_ALLPRF
#define  CFG_PRF
#define  CFG_NB_PRF						39
#define  CFG_EXT_DB
#define  CFG_CON_CTE_REQ
#define  CFG_CON_CTE_RSP
#define  CFG_CONLESS_CTE_TX
#define  CFG_CONLESS_CTE_RX
#define  CFG_AOA
#define  CFG_AOD
#define  CFG_AHITL
#define  CFG_HCITL
#define  CFG_PLF_UART
#define  CFG_PRF_ANPC
#define  CFG_PRF_ANPS
#define  CFG_PRF_BASC
#define  CFG_PRF_BASS
#define  CFG_PRF_BLPC
#define  CFG_PRF_BLPS
#define  CFG_PRF_CGMC
#define  CFG_PRF_CGMS
#define  CFG_PRF_CPPC
#define  CFG_PRF_CPPS
#define  CFG_PRF_CSCPC
#define  CFG_PRF_CSCPS
#define  CFG_PRF_DISC
#define  CFG_PRF_DISS
#define  CFG_PRF_FMPT
#define  CFG_PRF_FMPL
#define  CFG_PRF_GLPC
#define  CFG_PRF_GLPS
#define  CFG_PRF_HOGPBH
#define  CFG_PRF_HOGPD
#define  CFG_PRF_HOGPRH
#define  CFG_PRF_HRPC
#define  CFG_PRF_HRPS
#define  CFG_PRF_HTPC
#define  CFG_PRF_HTPT
#define  CFG_PRF_LANC
#define  CFG_PRF_LANS
#define  CFG_PRF_PASPC
#define  CFG_PRF_PASPS
#define  CFG_PRF_PXPM
#define  CFG_PRF_PXPR
#define  CFG_PRF_RSCPC
#define  CFG_PRF_RSCPS
#define  CFG_PRF_SCPPC
#define  CFG_PRF_SCPPS
#define  CFG_PRF_TIPC
#define  CFG_PRF_TIPS
#define  CFG_PRF_WPTS
#define  CFG_PRF_WPTC
#define  CFG_PRF_GLPS
#define  CFG_PRF_GLPC

#endif

#ifdef CFG_LIGHT_BUILD_CONFIG
#endif

#ifdef CFG_MESH_BUILD_CONFIG
#endif

#endif /* INCLUDE_BUILD_CONFIG_H_ */
