import { NavigationContainer } from "@react-navigation/native";
import { HomeTabs } from "./navigations/tab";
import { LoginStack } from "./navigations/stack";

export default function App() {
  const isLoggedIn = false;
  return (
    <NavigationContainer>
      <LoginStack />
    </NavigationContainer>
  );
}
