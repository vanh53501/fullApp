import { useNavigation } from "@react-navigation/native";
import { Text, SafeAreaView, Pressable, Image } from "react-native";

const ProfileScreen = () => {
  const navigation = useNavigation();
  return (
    <SafeAreaView
      style={{
        alignItems: "center",
        padding: 20,
        justifyContent: "space-around",
      }}
    >
      <Image
        source={require("../assets/image/user.png")}
        style={{ width: 150, height: 150 }}
      />
      <Pressable
        onPress={() => navigation.popToTop()}
        style={{
          marginBottom: 50,
          borderRadius: 10,
          borderWidth: 1,
          padding: 20,
        }}
      >
        <Text>Log Out</Text>
      </Pressable>
    </SafeAreaView>
  );
};

export default ProfileScreen;
