import { createStackNavigator } from "@react-navigation/stack";
import HomeScreen from "../screens/home-screen";
import DetailScreen from "../screens/detail-screen";
import ProfileScreen from "../screens/profile-screen";
import LoginScreen from "../screens/login-screen";
import { HomeTabs } from "./tab";

const Stack = createStackNavigator();

export const HomeStack = () => {
  return (
    <Stack.Navigator>
      <Stack.Screen
        name="Home"
        component={HomeScreen}
        options={{ headerShown: false }}
      />
      <Stack.Screen name="Detail" component={DetailScreen} />
    </Stack.Navigator>
  );
};

export const ProfileStack = () => {
  return (
    <Stack.Navigator screenOptions={{ headerShown: false }}>
      <Stack.Screen name="Info" component={ProfileScreen} />
      <Stack.Screen name="Login" component={LoginScreen} />
    </Stack.Navigator>
  );
};

export const LoginStack = () => {
  return (
    <Stack.Navigator screenOptions={{ headerShown: false }}>
      <Stack.Screen name="Login" component={LoginScreen} />
      <Stack.Screen name="Main" component={HomeTabs} />
    </Stack.Navigator>
  );
};
