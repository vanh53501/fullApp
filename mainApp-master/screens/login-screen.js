import React, { useState } from "react";
import {
  SafeAreaView,
  StyleSheet,
  ImageBackground,
  Image,
  Text,
} from "react-native";
import { Appbar, TextInput, Button } from "react-native-paper";
import { useNavigation } from "@react-navigation/native";

const LoginScreen = () => {
  const navigation = useNavigation();

  const [email, setEmail] = useState("");
  const [password, setPassword] = useState("");

  const handleLogin = () => {
    navigation.navigate("Main");
  };

  return (
    <SafeAreaView style={styles.container}>
      <ImageBackground
        source={require("../assets/images/bg.png")}
        style={styles.backgroundImage}
      >
        <SafeAreaView
          style={{
            justifyContent: "center",
            alignItems: "center",
            height: 140,
          }}
        >
          <Image
            style={{ width: 100, resizeMode: "contain" }}
            source={require("../assets/images/hydro.png")}
          />
        </SafeAreaView>
        <Appbar.Header style={styles.transparentHeader}>
          <Text style={styles.headerText}>App Login </Text>
        </Appbar.Header>
        <SafeAreaView style={styles.loginBox}>
          <TextInput
            label="Email"
            value={email}
            onChangeText={setEmail}
            style={styles.input}
          />
          <TextInput
            label="Password"
            value={password}
            onChangeText={setPassword}
            secureTextEntry
            style={styles.input}
          />
          <Button
            mode="contained"
            onPress={handleLogin}
            style={styles.button}
            labelStyle={styles.buttonLabel}
          >
            Sign in
          </Button>
        </SafeAreaView>
      </ImageBackground>
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
  },
  headerText: {
    fontSize: 24, // Cỡ chữ to hơn
    color: "white",
    fontWeight: "bold", // Đậm
  },
  logo: {
    width: 40,
    height: 40,
    marginRight: 10,
  },
  backgroundImage: {
    flex: 1,
    resizeMode: "cover",
    justifyContent: "center",
    alignItems: "center",
    width: "100%",
    height: "100%",
  },
  transparentHeader: {
    backgroundColor: "transparent",
    elevation: 0, // Loại bỏ đường viền bóng của Appbar.Header
  },
  loginBox: {
    width: "80%",
    alignItems: "center",
    backgroundColor: "rgba(255, 255, 255, 0)", // Background trong suốt
    padding: 20,
    borderRadius: 10,
  },
  title: {
    fontSize: 24,
    color: "white",
  },
  input: {
    width: "100%",
    marginVertical: 10,
    backgroundColor: "white",
  },
  button: {
    width: "100%",
    marginTop: 20,
    backgroundColor: "#002855",
    borderRadius: 5,
  },
  buttonLabel: {
    color: "white",
    fontSize: 16,
    fontWeight: "bold",
  },
  forgotPassword: {
    marginTop: 10,
    color: "#002855",
  },

  createAccount: {
    marginTop: 20,
    color: "black",
  },
  createAccountLink: {
    color: "#002855",
    fontWeight: "bold",
  },
});

export default LoginScreen;
