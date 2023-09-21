import { View, Text, FlatList, RefreshControl } from "react-native";
import { FAKE_DATA } from "../../data/fake-data";
import InfoItem from "./info-item";

const InfoList = () => {
  const renderItem = ({ item }) => {
    return (
      <InfoItem
        image={item.image}
        temp={item.temp}
        light={item.light}
        pH={item.pH}
        hum={item.hum}
        ec={item.ec}
        led={item.led}
      />
    );
  };
  return (
    <View>
      <FlatList data={FAKE_DATA} renderItem={renderItem} />
    </View>
  );
};

export default InfoList;
