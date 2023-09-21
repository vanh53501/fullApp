import {
  View,
  Text,
  FlatList,
  RefreshControl,
  SafeAreaView,
} from "react-native";
import { DUMMY_DATA } from "../../data/dummy";
import EventItem from "./event-item";

const EventList = () => {
  const renderItem = ({ item }) => {
    return (
      <EventItem
        id={item.id}
        title={item.title}
        description={item.description}
      />
    );
  };
  return (
    <SafeAreaView>
      <FlatList
        data={DUMMY_DATA}
        renderItem={renderItem}
        keyExtractor={(item) => item.id}
        numColumns={2}
        refreshControl={<RefreshControl refreshing={false} />}
      />
    </SafeAreaView>
  );
};

export default EventList;
