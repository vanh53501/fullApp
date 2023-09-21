import { useNavigation } from "@react-navigation/native";
import { TouchableOpacity, StyleSheet, Text, Image } from "react-native";

const EventItem = ({ id, title, description }) => {
  const navigation = useNavigation();
  return (
    <TouchableOpacity
      style={styles.card}
      onPress={() => navigation.navigate("Detail", { id, title, description })}
    >
      <Image
        style={{ width: 100, height: 100, borderRadius: 10 }}
        /* source={require("../../assets/tree.jpg")} */
        source={description}
      />
      <Text>{title}</Text>
      {/* <Text>{description}</Text> */}
    </TouchableOpacity>
  );
};

const styles = StyleSheet.create({
  card: {
    borderWidth: 1,
    borderColor: "#c5c5c5",
    borderRadius: 10,
    margin: 10,
    padding: 20,
    alignItems: "center",
    justifyContent: "space-around",
  },
});

export default EventItem;
