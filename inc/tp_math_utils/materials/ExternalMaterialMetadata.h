#ifndef tp_math_utils_ExternalMaterialMetadata_h
#define tp_math_utils_ExternalMaterialMetadata_h

#include "tp_math_utils/Globals.h"

#include "tp_utils/DebugUtils.h"
#include <type_traits>

#include <json.hpp>
#include <string>
#include <vector>
#include <memory>

#pragma push_macro("RGB")
#undef RGB

namespace tp_math_utils
{

//##################################################################################################
class ExternalMaterialVariable
{

public:
//##################################################################################################
  struct Data
  {
    inline static const std::string TypeStr = "invalid";
    
    virtual ~Data(){}

    bool operator==(const Data& other)
    {
      auto j = nlohmann::json();
      save(j);
      auto jother = nlohmann::json();
      other.save(jother);
      return (j.dump()==jother.dump() && type() == other.type());
    }

    virtual std::string type() const { return TypeStr; }
    virtual void load(const nlohmann::json&) {}
    virtual void save(nlohmann::json&) const {};
  };

//##################################################################################################
  struct Value : public ExternalMaterialVariable::Data
  {
    inline static const std::string TypeStr = "value";

    virtual ~Value(){}

    float value{0.0f};

    std::string type() const override { return TypeStr; }
    void load(const nlohmann::json& jData) override;
    void save(nlohmann::json& j) const override;
  };


//##################################################################################################
  struct RGB : public ExternalMaterialVariable::Data
  {
    inline static const std::string TypeStr = "rgb";

    virtual ~RGB(){}

    float r{0.0f};
    float g{0.0f};
    float b{0.0f};
    float a{0.0f};

    std::string type() const override { return TypeStr; }
    void load(const nlohmann::json& jData) override;
    void save(nlohmann::json& j) const override;
  };


//##################################################################################################
  ExternalMaterialVariable()
  {
    _data.reset(new ExternalMaterialVariable::Data());
  }

//##################################################################################################
  ~ExternalMaterialVariable() = default;

//##################################################################################################
  bool operator==(const ExternalMaterialVariable& other)
  {
    bool result = true;
    result &= (_name == other.name());
    result &= (*_data) == other.getData<ExternalMaterialVariable::Data>();

    return result;
  } 

//##################################################################################################
  ExternalMaterialVariable(const ExternalMaterialVariable& other)
  {
    *this = other;
  }

//##################################################################################################
  ExternalMaterialVariable& operator=(const ExternalMaterialVariable& other)
  {
    _name = other.name();

    if(other.type() == ExternalMaterialVariable::Value::TypeStr)
      _data.reset(new ExternalMaterialVariable::Value(other.getData<ExternalMaterialVariable::Value>()));
    else if(other.type() == ExternalMaterialVariable::RGB::TypeStr)
      _data.reset(new ExternalMaterialVariable::RGB(other.getData<ExternalMaterialVariable::RGB>()));
    else
      _data.reset(new ExternalMaterialVariable::Data());
    return *this;
  }

//##################################################################################################
  ExternalMaterialVariable(ExternalMaterialVariable&& other)
  {
    _name = std::move(other._name);
    _data = std::move(other._data);
  }

//##################################################################################################
  ExternalMaterialVariable& operator=(ExternalMaterialVariable&& other)
  {
    _name = std::move(other._name);
    _data = std::move(other._data);
    return *this;
  }

//##################################################################################################
  std::string name() const { return _name; }

//##################################################################################################
  std::string type() const { return _data->type(); }

//##################################################################################################
  std::string value() const { nlohmann::json j; _data->save(j); return j.dump(2); }
  
//##################################################################################################
 template <typename T>
 const T& getData() const
 {
   static_assert(std::is_base_of<ExternalMaterialVariable::Data, T>::value, "type parameter of this class must derive from ExternalMaterialVariable::Data");
   assert(T::TypeStr == _data->type());
   return *dynamic_cast<T*>(_data.get());
 }

//##################################################################################################
 template <typename T>
 void setData(const T& newData)
 {
   static_assert(std::is_base_of<ExternalMaterialVariable::Data, T>::value, "type parameter of this class must derive from ExternalMaterialVariable::Data");
   assert(T::TypeStr == _data->type());
   _data.reset(new T(newData));
 }

//##################################################################################################
  void loadState(const nlohmann::json& j);
//##################################################################################################
  void saveState(nlohmann::json& j) const;

private:
  std::string _name;
  std::unique_ptr<ExternalMaterialVariable::Data> _data;
};

//##################################################################################################
struct ExternalMaterialMetadata
{
  std::string name;
  std::vector<ExternalMaterialVariable> variables;

//##################################################################################################
  void loadState(const nlohmann::json& j);
//##################################################################################################
  void saveState(nlohmann::json& j) const;
};

//##################################################################################################
std::vector<ExternalMaterialMetadata> LoadMaterialMetadataFromFile(const std::string metadataFilePath);

//##################################################################################################
std::vector<ExternalMaterialMetadata> LoadMaterialMetadataFromTextJson(const std::string metadataDataJson);

//##################################################################################################
std::vector<ExternalMaterialMetadata> LoadMaterialMetadataFromJson(const nlohmann::json& metadataDataJson);

}

#pragma pop_macro("RGB")

#endif
