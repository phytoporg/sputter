defmodule Rooms.RegistryTest do
  use ExUnit.Case, async: true

  setup context do
    _ = start_supervised!({Rooms.Registry, name: context.test})
    %{registry: context.test}
  end

  test "can create an entry and look it up", %{registry: registry} do
    assert Rooms.Registry.lookup(registry, "shopping") == :error
    Rooms.Registry.create(registry, "shopping")
    assert {:ok, bucket} = Rooms.Registry.lookup(registry, "shopping")
    Rooms.Bucket.put(bucket, "milk", 1)
    assert Rooms.Bucket.get(bucket, "milk") == 1
  end

  test "removes buckets on exit", %{registry: registry} do
    Rooms.Registry.create(registry, "shopping")
    {:ok, bucket} = Rooms.Registry.lookup(registry, "shopping")
    Agent.stop(bucket)
    # Do a call to ensure the registry processed the DOWN message
    _ = Rooms.Registry.create(registry, "bogus")
    assert Rooms.Registry.lookup(registry, "shopping") == :error
  end

  test "should remain running even if a bucket crashes", %{registry: registry} do
    Rooms.Registry.create(registry, "shopping")
    {:ok, bucket} = Rooms.Registry.lookup(registry, "shopping")
    Agent.stop(bucket, :shutdown)
    # Do a call to ensure the registry processed the DOWN message
    _ = Rooms.Registry.create(registry, "bogus")
    assert Rooms.Registry.lookup(registry, "shopping") == :error
  end
end
