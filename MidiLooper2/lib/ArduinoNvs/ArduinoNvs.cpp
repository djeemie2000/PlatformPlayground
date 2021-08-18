// ArduinoNvs.cpp

// Copyright (c) 2018 Sinai RnD
// Copyright (c) 2016-2017 TridentTD

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "ArduinoNvs.h"

ArduinoNvs::ArduinoNvs()
{
}

bool ArduinoNvs::begin(String namespaceNvs)
{
  esp_err_t err = nvs_flash_init();
  if (err != ESP_OK)
  {
    DEBUG_PRINTLN("W: NVS. Cannot init flash mem");
    if (err != ESP_ERR_NVS_NO_FREE_PAGES)
      return false;

    // erase and reinit
    DEBUG_PRINTLN("NVS. Try reinit the partition");
    const esp_partition_t *nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    if (nvs_partition == NULL)
      return false;
    err = esp_partition_erase_range(nvs_partition, 0, nvs_partition->size);
    esp_err_t err = nvs_flash_init();
    if (err)
      return false;
    DEBUG_PRINTLN("NVS. Partition re-formatted");
  }

  err = nvs_open(namespaceNvs.c_str(), NVS_READWRITE, &_nvs_handle);
  if (err != ESP_OK)
    return false;

  return true;
}

void ArduinoNvs::close()
{
  nvs_close(_nvs_handle);
}

bool ArduinoNvs::eraseAll(bool forceCommit)
{
  esp_err_t err = nvs_erase_all(_nvs_handle);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::erase(String key, bool forceCommit)
{
  esp_err_t err = nvs_erase_key(_nvs_handle, key.c_str());
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::commit()
{
  esp_err_t err = nvs_commit(_nvs_handle);
  if (err != ESP_OK)
    return false;
  return true;
}

bool ArduinoNvs::setInt(String key, uint8_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_u8(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(String key, int16_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_i16(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(String key, uint16_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_u16(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(String key, int32_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_i32(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(String key, uint32_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_u32(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}
bool ArduinoNvs::setInt(String key, int64_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_i64(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setInt(String key, uint64_t value, bool forceCommit)
{
  esp_err_t err = nvs_set_u64(_nvs_handle, (char *)key.c_str(), value);
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setBool(String key, bool value, bool forceCommit)
{
  uint8_t val = value ? 0x01 : 0x00;
  return setInt(key, val, forceCommit);
}


bool ArduinoNvs::setString(String key, String value, bool forceCommit)
{
  esp_err_t err = nvs_set_str(_nvs_handle, (char *)key.c_str(), value.c_str());
  if (err != ESP_OK)
    return false;
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::setBlob(String key, uint8_t *blob, size_t length, bool forceCommit)
{
  DEBUG_PRINTF("ArduinoNvs::setObjct(): set obj addr = [0x%X], length = [%d]\n", (int32_t)blob, length);
  if (length == 0) 
    return false;
  esp_err_t err = nvs_set_blob(_nvs_handle, (char *)key.c_str(), blob, length);
  if (err)
  {
    DEBUG_PRINTF("ArduinoNvs::setObjct(): err = [0x%X]\n", err);
    return false;
  }
  return forceCommit ? commit() : true;
}

bool ArduinoNvs::getInt(String key, uint8_t& value)
{
  uint8_t v_u8 = 0;
  esp_err_t err = nvs_get_u8(_nvs_handle, (char *)key.c_str(), &v_u8);
  if (err == ESP_OK)
  { 
    value = v_u8;
    return true;
   }
   return false;
}

bool ArduinoNvs::getInt(String key, int16_t& value)
{
  int16_t v_u8 = 0;
  esp_err_t err = nvs_get_i16(_nvs_handle, (char *)key.c_str(), &v_u8);
  if (err == ESP_OK)
  { 
    value = v_u8;
    return true;
   }
   return false;
}

bool ArduinoNvs::getInt(String key, uint16_t& value)
{
  uint16_t v_u8 = 0;
  esp_err_t err = nvs_get_u16(_nvs_handle, (char *)key.c_str(), &v_u8);
  if (err == ESP_OK)
  { 
    value = v_u8;
    return true;
   }
   return false;
}

bool ArduinoNvs::getInt(String key, int32_t& value)
{
  int32_t v_u8 = 0;
  esp_err_t err = nvs_get_i32(_nvs_handle, (char *)key.c_str(), &v_u8);
  if (err == ESP_OK)
  { 
    value = v_u8;
    return true;
   }
   return false;
}

bool ArduinoNvs::getInt(String key, uint32_t& value)
{
  uint32_t v_u8 = 0;
  esp_err_t err = nvs_get_u32(_nvs_handle, (char *)key.c_str(), &v_u8);
  if (err == ESP_OK)
  { 
    value = v_u8;
    return true;
   }
   return false;
}

bool ArduinoNvs::getFloat(String key, float& value)
{
  size_t dummySize = 0;
  uint8_t res[sizeof(float)];
  if (getBlob(key, res, sizeof(float), dummySize))
  { 
    value =  *(float *)(res);
    return true;
  }

  return false;
}

bool ArduinoNvs::getBool(String key, bool& value)
{
  uint8_t val = 0;
  if(getInt(key, val))
  {
    value = (val != 0x00);
    return true;
  }
  return false;
}


// int64_t ArduinoNvs::getInt(String key, int64_t default_value)
// {
//   uint8_t v_u8;
//   int16_t v_i16;
//   uint16_t v_u16;
//   int32_t v_i32;
//   uint32_t v_u32;
//   int64_t v_i64;
//   uint64_t v_u64;

//   esp_err_t err;
//   err = nvs_get_u8(_nvs_handle, (char *)key.c_str(), &v_u8);
//   if (err == ESP_OK)
//     return (int64_t)v_u8;

//   err = nvs_get_i16(_nvs_handle, (char *)key.c_str(), &v_i16);
//   if (err == ESP_OK)
//     return (int64_t)v_i16;

//   err = nvs_get_u16(_nvs_handle, (char *)key.c_str(), &v_u16);
//   if (err == ESP_OK)
//     return (int64_t)v_u16;

//   err = nvs_get_i32(_nvs_handle, (char *)key.c_str(), &v_i32);
//   if (err == ESP_OK)
//     return (int64_t)v_i32;

//   err = nvs_get_u32(_nvs_handle, (char *)key.c_str(), &v_u32);
//   if (err == ESP_OK)
//     return (int64_t)v_u32;

//   err = nvs_get_i64(_nvs_handle, (char *)key.c_str(), &v_i64);
//   if (err == ESP_OK)
//     return (int64_t)v_i64;

//   err = nvs_get_u64(_nvs_handle, (char *)key.c_str(), &v_u64);
//   if (err == ESP_OK)
//     return (int64_t)v_u64;

//   return default_value;
// }

bool ArduinoNvs::getString(String key, String &res)
{
  size_t required_size;
  esp_err_t err;

  err = nvs_get_str(_nvs_handle, key.c_str(), NULL, &required_size);
  if (err)
    return false;

  char value[required_size];
  err = nvs_get_str(_nvs_handle, key.c_str(), value, &required_size);
  if (err)
    return false;
  res = value;
  return true;
}

bool ArduinoNvs::getBlobSize(String key, size_t& size)
{
  size_t required_size = 0;
  esp_err_t err = nvs_get_blob(_nvs_handle, key.c_str(), NULL, &required_size);
  if (err == ESP_OK)
  { 
    size = required_size;
    return true;
  }
  return false;
}

bool ArduinoNvs::getBlob(String key, uint8_t *blob, size_t capacity, size_t& size)
{
  if (capacity == 0)
    return false;

  size_t required_size = 0;
  if(!getBlobSize(key, required_size))
  {
    return false;
  }
  if (capacity < required_size)
  {
   return false;
  }

  esp_err_t err = nvs_get_blob(_nvs_handle, key.c_str(), blob, &required_size);
  if (err)
  {
    DEBUG_PRINTF("ArduinoNvs::getBlob(): get object err = [0x%X]\n", err);
    return false;
  }

  size = required_size;
  return true;
}

bool ArduinoNvs::setFloat(String key, float value, bool forceCommit)
{
  return setBlob(key, (uint8_t *)&value, sizeof(float), forceCommit);
}

ArduinoNvs NVS;
