import { View, Text, StyleSheet, Image, Linking } from "react-native";
import InfoList from "../components/info/info-list";
import { TouchableOpacity } from "react-native-gesture-handler";

const DetailScreen = () => {
  return (
    <View style={{ alignItems: "center", margin: 20 }}>
      {/* <TouchableOpacity
        onPress={() =>
          Linking.openURL(
            "https://vi.wikipedia.org/wiki/C%E1%BA%A3i_ng%E1%BB%8Dt"
          )
        }
      >
        <Image
          source={require("../assets/tree.jpg")}
          style={{ width: 300, height: 300, borderRadius: 10 }}
        />
      </TouchableOpacity> */}
      <InfoList />
    </View>
  );
};

const styles = StyleSheet.create({
  screen: {
    padding: 20,
  },
});

export default DetailScreen;
