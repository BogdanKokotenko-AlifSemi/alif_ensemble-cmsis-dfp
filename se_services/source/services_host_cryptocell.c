/**
 * @file services_host_cryptocell.c
 *
 * @brief Cryptocell services service source file
 * @ingroup host_services
 * @par
 *
 * Copyright (C) 2022 Alif Semiconductor - All Rights Reserved.
 * Use, distribution and modification of this code is permitted under the
 * terms stated in the Alif Semiconductor Software License Agreement
 *
 * You should have received a copy of the Alif Semiconductor Software
 * License Agreement with this file. If not, please write to:
 * contact@alifsemi.com, or visit: https://alifsemi.com/license
 *
 */

/******************************************************************************
 *  I N C L U D E   F I L E S
 *****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "services_lib_api.h"
#include "services_lib_protocol.h"
#include "services_lib_ids.h"
#if defined(A32_LINUX)
#include "a32_linux.h"
#else
#include "system_utils.h"
#endif

/*******************************************************************************
 *  M A C R O   D E F I N E S
 ******************************************************************************/

#define UNUSED(x) (void)(x)

/*******************************************************************************
 *  T Y P E D E F S
 ******************************************************************************/

/*******************************************************************************
 *  G L O B A L   V A R I A B L E S
 ******************************************************************************/

/*******************************************************************************
 *  C O D E
 ******************************************************************************/

/**
 * @brief Services Crypto get TRNG
 * @param services_handle
 * @param rnd_len
 * @param rnd_value
 * @return
 */
uint32_t SERVICES_cryptocell_get_rnd(uint32_t services_handle, 
                                     uint16_t rnd_len,
                                     void * rnd_value,
                                     int32_t * error_code)
{
  get_rnd_svc_t * p_svc = (get_rnd_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(get_rnd_svc_t));

  p_svc->send_rnd_length = rnd_len;
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_GET_RND,
                                       DEFAULT_TIMEOUT);
  memcpy(rnd_value, (const void *)p_svc->resp_rnd, rnd_len);
  *error_code = p_svc->resp_error_code;
  return err;
}

/**
 * @brief Services Crypto get LCS State
 * @param services_handle
 * @param lcs_state
 * @return
 */
uint32_t SERVICES_cryptocell_get_lcs(uint32_t services_handle, 
                                     uint32_t * lcs_state,
                                     int32_t * error_code)
{
  get_lcs_svc_t * p_svc = (get_lcs_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(get_lcs_svc_t));
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_GET_LCS,
                                       DEFAULT_TIMEOUT);
  *lcs_state = p_svc->resp_lcs;
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_hardware_poll(uint32_t services_handle,
                                                   uint32_t * error_code,
                                                   uint32_t data,
                                                   uint32_t output,
                                                   uint32_t len,
                                                   uint32_t olen)
{
  mbedtls_trng_hardware_poll_svc_t * p_svc = (mbedtls_trng_hardware_poll_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_trng_hardware_poll_svc_t));

  p_svc->send_data_addr = LocalToGlobal((void *)data);
  p_svc->send_output_addr = LocalToGlobal((void *)output);
  p_svc->send_olen_addr = LocalToGlobal((void *)olen);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_TRNG_HARDWARE_POLL,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(len);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_aes_init(uint32_t services_handle, 
                                              uint32_t * error_code, 
                                              uint32_t ctx)
{
  mbedtls_aes_init_svc_t * p_svc = (mbedtls_aes_init_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_aes_init_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_AES_INIT,
                                       DEFAULT_TIMEOUT);
  *error_code = 0;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_aes_set_key(uint32_t services_handle, 
                                                 uint32_t * error_code, 
                                                 uint32_t ctx,
                                                 uint32_t key,
                                                 uint32_t keybits,
                                                 uint32_t dir)
{
  mbedtls_aes_set_key_svc_t * p_svc = (mbedtls_aes_set_key_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_aes_set_key_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  p_svc->send_key_addr = LocalToGlobal((void *)key);
  p_svc->send_key_bits = keybits;
  p_svc->send_direction = dir;
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_AES_SET_KEY,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_aes_crypt(uint32_t services_handle, 
                                               uint32_t * error_code,
                                               uint32_t ctx,
                                               uint32_t crypt_type, 
                                               uint32_t mode, 
                                               uint32_t length, 
                                               uint32_t iv, 
                                               uint32_t input, 
                                               uint32_t output)
{
  mbedtls_aes_crypt_svc_t * p_svc = (mbedtls_aes_crypt_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_aes_crypt_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  p_svc->send_crypt_type = crypt_type;
  p_svc->send_mode = mode;
  p_svc->send_length = length;
  p_svc->send_iv_addr = LocalToGlobal((void *)iv);
  p_svc->send_input_addr = LocalToGlobal((void *)input);
  p_svc->send_output_addr = LocalToGlobal((void *)output);
  
  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_AES_CRYPT,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_starts(uint32_t services_handle, 
                                                uint32_t * error_code, 
                                                uint32_t ctx, 
                                                uint32_t sha_type)
{
  mbedtls_sha_svc_t * p_svc = (mbedtls_sha_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_sha_svc_t));
  
  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  p_svc->send_sha_type = sha_type;

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_STARTS,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_process(uint32_t services_handle, 
                                                 uint32_t * error_code, 
                                                 uint32_t ctx, 
                                                 uint32_t sha_type, 
                                                 uint32_t data)
{
  mbedtls_sha_svc_t * p_svc = (mbedtls_sha_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_sha_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  p_svc->send_sha_type = sha_type;
  p_svc->send_data_addr = LocalToGlobal((void *)data);

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_PROCESS,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_update(uint32_t services_handle, 
                                                uint32_t * error_code, 
                                                uint32_t ctx, 
                                                uint32_t sha_type, 
                                                uint32_t data, 
                                                uint32_t data_length)
{
  mbedtls_sha_svc_t * p_svc = (mbedtls_sha_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_sha_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  p_svc->send_sha_type = sha_type;
  p_svc->send_data_addr = LocalToGlobal((void *)data);
  p_svc->send_data_length = data_length;

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_UPDATE,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_sha_finish(uint32_t services_handle, 
                                                uint32_t * error_code, 
                                                uint32_t ctx, 
                                                uint32_t sha_type, 
                                                uint32_t data)
{
  mbedtls_sha_svc_t * p_svc = (mbedtls_sha_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_sha_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)ctx);
  p_svc->send_sha_type = sha_type;
  p_svc->send_data_addr = LocalToGlobal((void *)data);

  uint32_t err = SERVICES_send_request(services_handle, 
                                       SERVICE_CRYPTOCELL_MBEDTLS_SHA_FINISH,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_ccm_gcm_set_key(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr,
    uint32_t key_type,
    uint32_t cipher,
    uint32_t key_addr,
    uint32_t key_bits)
{
  mbedtls_ccm_gcm_set_key_svc_t * p_svc = (mbedtls_ccm_gcm_set_key_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_ccm_gcm_set_key_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);
  p_svc->send_key_addr = LocalToGlobal((void *)key_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CCM_GCM_SET_KEY,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(key_type);
  UNUSED(cipher);
  UNUSED(key_bits);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_ccm_gcm_crypt(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr,
    uint32_t crypt_type,
    uint32_t length,
    uint32_t iv_addr,
    uint32_t iv_length,
    uint32_t add_addr,
    uint32_t add_length,
    uint32_t input_addr,
    uint32_t output_addr,
    uint32_t tag_addr,
    uint32_t tag_length)
{
  mbedtls_ccm_gcm_crypt_svc_t * p_svc = (mbedtls_ccm_gcm_crypt_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_ccm_gcm_crypt_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);
  p_svc->send_iv_addr = LocalToGlobal((void *)iv_addr);
  p_svc->send_add_addr = LocalToGlobal((void *)add_addr);
  p_svc->send_input_addr = LocalToGlobal((void *)input_addr);
  p_svc->send_output_addr = LocalToGlobal((void *)output_addr);
  p_svc->send_tag_addr = LocalToGlobal((void *)tag_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CCM_GCM_CRYPT,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(crypt_type);
  UNUSED(length);
  UNUSED(iv_length);
  UNUSED(add_length);
  UNUSED(tag_length);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_chacha20_crypt(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t key_addr,
    uint32_t nonce_addr,
    uint32_t counter,
    uint32_t data_len,
    uint32_t input_addr,
    uint32_t output_addr)
{
  mbedtls_chacha20_crypt_svc_t * p_svc = (mbedtls_chacha20_crypt_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_chacha20_crypt_svc_t));

  p_svc->send_key_addr = LocalToGlobal((void *)key_addr);
  p_svc->send_nonce_addr = LocalToGlobal((void *)nonce_addr);
  p_svc->send_input_addr = LocalToGlobal((void *)input_addr);
  p_svc->send_output_addr = LocalToGlobal((void *)output_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CHACHA20_CRYPT,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(counter);
  UNUSED(data_len);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_chachapoly_crypt(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr,
    uint32_t crypt_type,
    uint32_t length,
    uint32_t nonce_addr,
    uint32_t aad_addr,
    uint32_t aad_len,
    uint32_t tag_addr,
    uint32_t input_addr,
    uint32_t output_addr)
{
  mbedtls_chachapoly_crypt_svc_t * p_svc = (mbedtls_chachapoly_crypt_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_chachapoly_crypt_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);
  p_svc->send_nonce_addr = LocalToGlobal((void *)nonce_addr);
  p_svc->send_aad_addr = LocalToGlobal((void *)aad_addr);
  p_svc->send_input_addr = LocalToGlobal((void *)input_addr);
  p_svc->send_tag_addr = LocalToGlobal((void *)tag_addr);
  p_svc->send_output_addr = LocalToGlobal((void *)output_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CHACHAPOLY_CRYPT,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(crypt_type);
  UNUSED(length);
  UNUSED(aad_len);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_poly1305_crypt(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t key_addr,
    uint32_t input_addr,
    uint32_t ilen,
    uint32_t mac_addr)
{
  mbedtls_poly1305_crypt_svc_t * p_svc = (mbedtls_poly1305_crypt_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_poly1305_crypt_svc_t));

  p_svc->send_key_addr = LocalToGlobal((void *)key_addr);
  p_svc->send_input_addr = LocalToGlobal((void *)input_addr);
  p_svc->send_mac_addr = LocalToGlobal((void *)mac_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_POLY1305_CRYPT,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(ilen);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_cmac_init_setkey(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr,
    uint32_t key_addr,
    uint32_t key_bits)
{
  mbedtls_cmac_init_setkey_svc_t * p_svc = (mbedtls_cmac_init_setkey_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_cmac_init_setkey_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);
  p_svc->send_key_addr = LocalToGlobal((void *)key_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CMAC_INIT_SETKEY,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(key_bits);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_cmac_update(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr,
    uint32_t input_addr,
    uint32_t input_length)
{
  mbedtls_cmac_update_svc_t * p_svc = (mbedtls_cmac_update_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_cmac_update_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);
  p_svc->send_input_addr = LocalToGlobal((void *)input_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CMAC_UPDATE,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  UNUSED(input_length);
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_cmac_finish(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr,
    uint32_t output_addr)
{
  mbedtls_cmac_finish_svc_t * p_svc = (mbedtls_cmac_finish_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_cmac_finish_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);
  p_svc->send_output_addr = LocalToGlobal((void *)output_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CMAC_FINISH,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}

uint32_t SERVICES_cryptocell_mbedtls_cmac_reset(uint32_t services_handle,
    uint32_t * error_code,
    uint32_t context_addr)
{
  mbedtls_cmac_reset_svc_t * p_svc = (mbedtls_cmac_reset_svc_t *)
    SERVICES_prepare_packet_buffer(sizeof(mbedtls_cmac_reset_svc_t));

  p_svc->send_context_addr = LocalToGlobal((void *)context_addr);

  uint32_t err = SERVICES_send_request(services_handle,
                                       SERVICE_CRYPTOCELL_MBEDTLS_CMAC_RESET,
                                       DEFAULT_TIMEOUT);
  *error_code = p_svc->resp_error_code;
  return err;
}
