/**
 * \file secril-client-compat.h
 * \brief libsecril-client compatibility library for FreeRIL-I9100.
 */
/*
 * Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __SECRIL_CLIENT_COMPAT__H__
#define __SECRIL_CLIENT_COMPAT__H__

#include <string.h>

/**
 * \brief Function call result.
 */

typedef enum ril_client_status {
    RIL_CLIENT_SUCCESS     = 0, ///< Call succeeded.
    RIL_CLIENT_INVALID     = 3, ///< Invalid connection context.
    RIL_CLIENT_REFUSED     = 4, ///< RIL not available or context isn't connected.
    RIL_CLIENT_IO_ERROR    = 5, ///< Input/output error.
    RIL_CLIENT_RES_UNAVAIL = 6, ///< Insufficient system resources.
    RIL_CLIENT_UNKNOWN     = 7  ///< Unknown error ocurred.
} ril_client_status_t;

/**
 * \brief Opaque RIL daemon connection context.
 */
typedef struct ril_client ril_client_t;

/**
 * \brief Unsolicited response callback.
 *
 * \param [in] code Unsolicited response code,
 * \param [in] response Response buffer,
 * \param [in] response_size Response buffer size.
 */
typedef void (*ril_client_unsolicited_callback_t)(int code, const void *response,
                                                   size_t response_size);

/**
 * \brief Request completion callback.
 *
 * \param [in] client Connection context,
 * \param [in] response Response buffer,
 * \param [in] response_size Response buffer size.
 */
typedef void (*ril_client_complete_callback_t)(ril_client_t *client, const void *response,
                                                size_t response_size);

/**
 * \brief Error callback.
 *
 * \param [in] error Error code,
 * \param [in] arg Parameter specified in call to \ref RegisterErrorHandler.
 */
typedef void (*ril_client_error_callback_t)(int error, void *arg);

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * \brief Create RIL daemon connection context.
 *
 * \return NULL on error, allocated context otherwise.
 */
ril_client_t *OpenClient_RILD(void);

/**
 * \brief Destroy RIL daemon connection context.
 *
 * Connection will be closed if open.
 *
 * \param [in] client Connection context.
 * \return Client status.
 */
ril_client_status_t CloseClient_RILD(ril_client_t *client);

/**
 * \brief Connect to RIL daemon.
 *
 * Connection must be closed before call.
 *
 * \param [in] client Connection context.
 * \return Client status.
 */
ril_client_status_t Connect_RILD(ril_client_t *client);

/**
 * \brief Disconnect from RIL daemon.
 *
 * Connection must be open before call.
 *
 * \param [in] client Connection context.
 * \return Client status.
 */
ril_client_status_t Disconnect_RILD(ril_client_t *client);

/**
 * \brief Check connection state.
 *
 * \param [in] client Connection context.
 * \return Client status.
 */
ril_client_status_t isConnected_RILD(ril_client_t *client);

/**
 *\brief Install unsolicited response handler.

 * Handler will be called upon reception of any unsolicited response with specified code
 * without any thread guarantees.
 *
 * \param [in] client Connection context,
 * \param [in] code RIL unsolicited response code,
 * \param [in] callback Callback function,
 * \return Client status.
 */
ril_client_status_t RegisterUnsolicitedHandler(ril_client_t *client, int code,
                                               ril_client_unsolicited_callback_t callback);

/**
 * \brief Register a completion handler for request type.
 *
 * Handler will be called upon completion of any request with specified code
 * without any thread guarantees.
 *
 * \param [in] client Connection context,
 * \param [in] code RIL request code,
 * \param [in] callback Completion callback.
 * \return Client status.
 */
ril_client_status_t  RegisterRequestCompleteHandler(ril_client_t *client, int code,
                                                    ril_client_complete_callback_t callback);

/**
 * \brief Register an error handler.
 *
 * \param [in] client Connection context,
 * \param [in] callback Error callback,
 * \param [in] arg Extra callback parameter.
 * \return Client status.
 */
ril_client_status_t RegisterErrorCallback(ril_client_t *client, ril_client_error_callback_t callback,
                                          void *arg);

/**
 * \brief Send OEM request to the RIL.
 *
 * A completion handler must be installed through \ref RegisterRequestCompleteHandler
 * before call to this function.
 *
 * \param [in] client Connection context,
 * \param [in] data Message data,
 * \param [in] data_size Message data size.
 * \return Client status.
 */
ril_client_status_t InvokeOemRequestHookRaw(ril_client_t *client, const void *data, size_t data_size);

/**
 * \brief Set audio loopback test mode.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * Set loopback to nonzero to start test and to zero to stop it.
 *
 * \param [in] client Connection context,
 * \param [in] loopback Test mode.
 * \param [in] path
 * \return Client status.
 */
ril_client_status_t SetLoopbackTest(ril_client_t *client, int loopback, int path);

/**
 * \brief Set DHA solution.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] mode TBD,
 * \param [in] select TBD.
 * \return Client status.
 */
ril_client_status_t SetDhaSolution(ril_client_t *client, int mode, int select, const char *extra);

/**
 * \brief Control dual-microphone processing.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] param1 TBD,
 * \param [in] param2 TBD.
 * \return Client status.
 */
ril_client_status_t SetTwoMicControl(ril_client_t *client, int param1, int param2);

/**
 * \brief Get mute status.
 *
 * \param [in] client Connection context,
 * \param [in] callback Completion callback.
 * \return Client status.
 */
ril_client_status_t GetMute(ril_client_t *client, ril_client_complete_callback_t callback);

/**
 * \brief Set mute status.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] mute Mute status.
 * \return Client status.
 */
ril_client_status_t SetMute(ril_client_t *client, int mute);

/**
 * \brief Set call recording status.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] record Call recording status status.
 * \return Client status.
 */
ril_client_status_t SetCallRecord(ril_client_t *client, int record);

/**
 * \brief Set call clock synchronization.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] sync
 * \return Client status.
 */
ril_client_status_t SetCallClockSync(ril_client_t *client, int sync);

/**
 * \brief Set video call clock synchronization.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] sync
 * \return Client status.
 */
ril_client_status_t SetVideoCallClockSync(ril_client_t *client, int sync);

/**
 * \brief Set call audio path.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] path
 * \return Client status.
 */
ril_client_status_t SetCallAudioPath(ril_client_t *client, int path,
    int extraVolume);

/**
 * \brief Set call volume.
 *
 * Request will be run asynchronously without any indication of completion.
 *
 * \param [in] client Connection context,
 * \param [in] device Destination device,
 * \param [in] volume Volume level.
 * \return Client status.
 */
ril_client_status_t SetCallVolume(ril_client_t *client, int device, int volume);

#if defined(__cplusplus)
}
#endif

#endif
